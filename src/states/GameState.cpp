#include "states/GameState.h"
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include "ui/WoodPanel.h"
#include "chess/GameSnapshot.h"
#include "chess/AIEngine.h"
#include "ui/EvalBar.h"
#include <thread>
#include <atomic>
#include <future>
#include "chess/PGNManager.h"

#include <stdexcept>

// Inicialización
void GameState::initVariables() {
	this->paused = false;
	this->turn = true;
	this->previousTurn = true;

	// Nombres y opciones provienen de la configuración elegida en GameSetupState.
	this->player1 = this->config.whiteName;
	this->player2 = this->config.blackName;
	this->points1 = 0;
	this->points2 = 0;
	this->gameOverReady = false;
	this->mouseHeldLastFrame = false;
	this->background.setPosition(sf::Vector2f(820.f, 0.f));

	this->baseTime = this->config.baseTime;
	this->increment = this->config.increment;
	this->aiMode = this->config.aiMode;
	this->aiDifficulty = this->config.aiDifficulty;

	// Orientación y bando de la IA: si el jugador lleva negras frente a la IA,
	// el tablero se voltea y la IA juega con blancas.
	this->flipped = this->config.aiMode && !this->config.playerIsWhite;
	this->aiPlaysWhite = this->flipped;
	// Fija la orientación de la vista antes de crear el tablero y las piezas,
	// que calculan su posición en píxeles según este flag.
	setBoardFlipped(this->flipped);

	this->timeWhite = this->baseTime;
	this->timeBlack = this->baseTime;
	this->clockStarted = false;

	this->aiIsThinking = false;
	this->aiStopFlag = false;

	this->confirmResignActive = false;
	this->drawOfferActive = false;
	this->drawRejectedActive = false;

	// Mensaje fin del juego
	this->gameOverBox = std::make_unique<MessageBox>(this->panelFont, "Fin de la partida", "Salir");
	this->confirmResignBox = std::make_unique<MessageBox>(this->panelFont, "Quieres rendirte?", "Si", "No");
	this->drawOfferBox = std::make_unique<MessageBox>(this->panelFont, "Ofrecen tablas.\nAceptas el empate?", "Aceptar", "Rechazar");
	this->drawRejectedBox = std::make_unique<MessageBox>(this->panelFont, "Tablas rechazadas\nEscafandrin no perdona", "Continuar");
}   
   

void GameState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Factory LJDS.ttf"))
		throw std::runtime_error("ERROR::GAME_STATE::COULD NOT LOAD FONT");
	if (!this->panelFont.loadFromFile("resources/fonts/Gameplay.ttf"))
		throw std::runtime_error("ERROR::GAME_STATE::COULD NOT LOAD PANEL FONT");
}

void GameState::initTextures() {
	// Botones
	if (!this->textures["BUTTONS"].loadFromFile("resources/images/interface/buttons.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD BUTTONS");
	}
	// Fondo
	if (!this->textures["BACKGROUND"].loadFromFile("resources/images/interface/background.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD BACKGROUND");
	}
	this->background.setTexture(this->textures["BACKGROUND"]);
	this->background.scale(3.067f, 2.0f);
}

void GameState::initPauseMenu() {
	this->pauseMenu = std::make_unique<PauseMenu>(this->panelFont);
	this->pauseMenu->addButton("EXIT", 50.f, 370.f, "Salir");
	this->pauseMenu->addButton("SAVE_PGN", 50.f, 270.f, "Guardar PGN");
	this->pauseMenu->addButton("CONTINUE", 50.f, 170.f, "Continuar");
}

void GameState::initGamePanel() {
	// ── Layout constants ──────────────────────────────────────────────────
	constexpr float W = 1280.f, H = 820.f;
	// Full dark background
	bgRect.setSize({W, H});
	bgRect.setFillColor(sf::Color(26, 12, 6));

	// Radial vignette
	constexpr int N = 32;
	vignetteVA.setPrimitiveType(sf::TriangleFan);
	vignetteVA.resize(N + 2);
	float cx = 640.f, cy = H * 0.5f;
	vignetteVA[0].position = {cx, cy};
	vignetteVA[0].color    = sf::Color(7, 2, 0, 0);
	float R = std::sqrt(W * W + H * H) * 0.6f;
	for (int i = 0; i <= N; ++i) {
		float a = (float(i) / N) * 2.f * 3.14159265f;
		vignetteVA[i+1].position = {cx + std::cos(a) * R, cy + std::sin(a) * R};
		vignetteVA[i+1].color    = sf::Color(7, 2, 0, 204);
	}

	// ── Side panel ────────────────────────────────────────────────────────
	constexpr float SX = 822.f, SY = 6.f, SW = 452.f, SH = 808.f;
	sidePanel.setBounds({SX, SY, SW, SH});

	// ── Board labels ──────────────────────────────────────────────────────
	auto setupLabel = [&](sf::Text& t, const std::string& s, sf::RectangleShape& dot,
	                      float x, float y, bool white) {
		t.setFont(panelFont);
		t.setString(s);
		t.setCharacterSize(10);
		t.setLetterSpacing(2.f);
		t.setFillColor(white ? sf::Color(240,220,170,166) : sf::Color(210,175,120,128));
		t.setPosition(x, y);
		dot.setSize({7.f, 7.f});
		dot.setPosition(x - 12.f, y + 2.f);
		dot.setFillColor(white ? sf::Color(237, 224, 196) : sf::Color(42, 26, 16));
		dot.setOutlineColor(sf::Color(200, 152, 72, 102));
		dot.setOutlineThickness(1.f);
	};
	// NEGRAS/BLANCAS según orientación.
	// Si el tablero está volteado (perspectiva de negras), BLANCAS arriba-izq y NEGRAS abajo-der.
	if (this->flipped) {
		setupLabel(labelBlancas, "BLANCAS", dotBlancas, 33.f, 4.f, true);
		setupLabel(labelNegras, "NEGRAS", dotNegras, 0.f, 804.f, false);
		{
			auto lb = labelNegras.getLocalBounds();
			float xText = 800.f - 7.f - 8.f - lb.width - lb.left;
			labelNegras.setPosition(xText, 804.f);
			dotNegras.setPosition(xText + lb.left + lb.width + 8.f, 804.f + 2.f);
		}
	} else {
		setupLabel(labelNegras, "NEGRAS", dotNegras, 33.f, 4.f, false);
		setupLabel(labelBlancas, "BLANCAS", dotBlancas, 0.f, 804.f, true);
		{
			auto lb = labelBlancas.getLocalBounds();
			float xText = 800.f - 7.f - 8.f - lb.width - lb.left;
			labelBlancas.setPosition(xText, 804.f);
			dotBlancas.setPosition(xText + lb.left + lb.width + 8.f, 804.f + 2.f);
		}
	}

	// ── Helper: absolute content coords ──────────────────────────────────
	// content X starts inside panel with 14px horizontal pad
	constexpr float CX = SX + 3.f + 14.f; // 839
	constexpr float CW = SW - 6.f - 28.f;  // 424

	// ── Black player row (top) ────────────────────────────────────────────
	constexpr float BLACK_Y = 21.f;
	blackRowBg.setSize({SW - 6.f, 62.f});
	blackRowBg.setPosition(SX + 3.f, BLACK_Y);
	blackRowBg.setFillColor(sf::Color(20, 10, 4, 71));
	blackRowBg.setOutlineColor(sf::Color(200, 148, 70, 30));
	blackRowBg.setOutlineThickness(1.f);

	blackKingBox.setSize({38.f, 38.f});
	blackKingBox.setPosition(CX, BLACK_Y + 12.f);
	blackKingBox.setFillColor(sf::Color(42, 26, 12));
	blackKingBox.setOutlineColor(sf::Color(200, 148, 70, 51));
	blackKingBox.setOutlineThickness(1.f);
	this->blackKingSprite.setTexture(this->textures["KN"]);
	this->blackKingSprite.setScale(30.f / 100.f, 30.f / 100.f);
	this->blackKingSprite.setPosition(CX + 4.f, BLACK_Y + 12.f + 4.f);

	blackNameTxt.setFont(panelFont);
	blackNameTxt.setString(this->player2);
	blackNameTxt.setCharacterSize(11);
	blackNameTxt.setLetterSpacing(1.8f);
	blackNameTxt.setFillColor(sf::Color(221, 208, 176));
	blackNameTxt.setPosition(CX + 46.f, BLACK_Y + 14.f);

	blackStatusTxt.setFont(panelFont);
	blackStatusTxt.setString("EN ESPERA");
	blackStatusTxt.setCharacterSize(9);
	blackStatusTxt.setLetterSpacing(1.2f);
	blackStatusTxt.setFillColor(sf::Color(200, 160, 100, 107));
	blackStatusTxt.setPosition(CX + 46.f, BLACK_Y + 34.f);

	blackTimerTxt.setFont(panelFont);
	blackTimerTxt.setString("--:--");
	blackTimerTxt.setCharacterSize(20);
	blackTimerTxt.setLetterSpacing(1.5f);
	blackTimerTxt.setFillColor(sf::Color(210, 175, 110, 122));
	auto btlb = blackTimerTxt.getLocalBounds();
	blackTimerTxt.setPosition(SX + SW - 3.f - 14.f - btlb.width - btlb.left, BLACK_Y + 20.f);

	// ── White player row (bottom) ─────────────────────────────────────────
	constexpr float WHITE_Y = 680.f;
	whiteRowBg.setSize({SW - 6.f, 62.f});
	whiteRowBg.setPosition(SX + 3.f, WHITE_Y);
	whiteRowBg.setFillColor(sf::Color(200, 168, 90, 26));
	whiteRowBg.setOutlineColor(sf::Color(202, 168, 90, 71));
	whiteRowBg.setOutlineThickness(1.f);

	turnBar.setSize({3.f, 62.f});
	turnBar.setPosition(SX + 3.f, WHITE_Y);
	turnBar.setFillColor(sf::Color(202, 164, 72));

	whiteKingBox.setSize({38.f, 38.f});
	whiteKingBox.setPosition(CX, WHITE_Y + 12.f);
	whiteKingBox.setFillColor(sf::Color(200, 184, 136));
	whiteKingBox.setOutlineColor(sf::Color(202, 168, 90, 89));
	whiteKingBox.setOutlineThickness(1.f);
	this->whiteKingSprite.setTexture(this->textures["KB"]);
	this->whiteKingSprite.setScale(30.f / 100.f, 30.f / 100.f);
	this->whiteKingSprite.setPosition(CX + 4.f, WHITE_Y + 12.f + 4.f);

	whiteNameTxt.setFont(panelFont);
	whiteNameTxt.setString(this->player1);
	whiteNameTxt.setCharacterSize(11);
	whiteNameTxt.setLetterSpacing(1.8f);
	whiteNameTxt.setFillColor(sf::Color(242, 228, 194));
	whiteNameTxt.setPosition(CX + 46.f, WHITE_Y + 14.f);

	whiteStatusTxt.setFont(panelFont);
	whiteStatusTxt.setString("TU TURNO");
	whiteStatusTxt.setCharacterSize(9);
	whiteStatusTxt.setLetterSpacing(1.2f);
	whiteStatusTxt.setFillColor(sf::Color(202, 168, 90, 173));
	whiteStatusTxt.setPosition(CX + 46.f, WHITE_Y + 34.f);

	whiteTimerTxt.setFont(panelFont);
	whiteTimerTxt.setString("--:--");
	whiteTimerTxt.setCharacterSize(20);
	whiteTimerTxt.setLetterSpacing(1.5f);
	whiteTimerTxt.setFillColor(sf::Color(202, 170, 88));
	auto wtlb = whiteTimerTxt.getLocalBounds();
	whiteTimerTxt.setPosition(SX + SW - 3.f - 14.f - wtlb.width - wtlb.left, WHITE_Y + 20.f);

	// ── Separator lines ───────────────────────────────────────────────────
	const float SEP_X = SX + 3.f;
	const float SEP_W = SW - 6.f;
	const sf::Color sepC(200, 148, 70, 50);
	auto setSep = [&](int idx, float y) {
		sepLines[idx].setSize({SEP_W, 1.f});
		sepLines[idx].setPosition(SEP_X, y);
		sepLines[idx].setFillColor(sepC);
	};
	setSep(0, BLACK_Y + 62.f + 5.f);   // below black row
	setSep(1, 91.f + 26.f + 3.f);      // below cap black
	setSep(2, 124.f + 518.f + 3.f);    // below move list
	setSep(3, 648.f + 26.f + 3.f);     // below cap white
	setSep(4, WHITE_Y + 62.f + 7.f);   // below white row

	// ── Advantage texts ───────────────────────────────────────────────────
	auto setupAdv = [&](sf::Text& t, float x, float y) {
		t.setFont(panelFont);
		t.setString("+0");
		t.setCharacterSize(13);
		t.setLetterSpacing(1.2f);
		t.setFillColor(sf::Color(210, 175, 110, 200));
		t.setPosition(x, y);
	};
	setupAdv(blackAdvTxt, SX + SW - 3.f - 14.f - 20.f, 93.f);
	setupAdv(whiteAdvTxt, SX + SW - 3.f - 14.f - 20.f, 650.f);

	// ── Action buttons ────────────────────────────────────────────────────
	constexpr float BTN_Y   = 752.f;
	constexpr float BTN_H   = 40.f;
	constexpr float BTN_GAP = 8.f;
	float bw = (CW - BTN_GAP) * 0.5f;
	actionBtns.emplace_back(CX,            BTN_Y, bw, BTN_H, &panelFont, "RENDIRSE", 12u);
	actionBtns.emplace_back(CX + bw + BTN_GAP, BTN_Y, bw, BTN_H, &panelFont, "OFRECER TABLAS", 12u);

	// ── EvalBar ───────────────────────────────────────────────────────────
	this->evalBar = std::make_unique<EvalBar>(839.f, 124.f, 20.f, 518.f, this->panelFont);
	this->evalBar->setFlipped(this->flipped);
	this->lastEvalScore = 0.0f;
	if (this->board) {
		this->launchAsyncEvaluation();
	}

	mouseHeldForActionBtns = false;
}

void GameState::initBoard(std::map<std::string, sf::Texture>& textures) {
	this->board = std::make_unique<Board>(textures);
	this->currentMoveIndex = -1;
	this->captureStateForUndo();
}

static std::string formatTime(float t) {
	if (t < 0) t = 0;
	int minutes = static_cast<int>(t) / 60;
	int seconds = static_cast<int>(t) % 60;
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
	return oss.str();
}

std::string GameState::buildScoreText() const {
	if (this->baseTime == 0.0f) {
		return "Black: " + this->player2 + "\nPoints: " + std::to_string(this->points2) +
			"\n\nWhite: " + this->player1 + "\nPoints: " + std::to_string(this->points1);
	}
	return "Black: " + this->player2 + "\nPoints: " + std::to_string(this->points2) +
		"\nTime: " + formatTime(this->timeBlack) + 
		"\n\nWhite: " + this->player1 + "\nPoints: " + std::to_string(this->points1) +
		"\nTime: " + formatTime(this->timeWhite);
}

void GameState::initText() {

	this->gameInfoText.setFont(this->font);
	this->gameInfoText.setCharacterSize(35);
	this->gameInfoText.setFillColor(sf::Color(75, 53, 47, 255));
	this->gameInfoText.setString(this->buildScoreText());
	this->gameInfoText.setPosition(850, 50);

	this->moveListPanel = std::make_unique<MoveListPanel>(this->panelFont, sf::FloatRect(865.f, 124.f, 398.f, 518.f), true);
}

void GameState::initKeybinds() {
	// Obtiene los keybinds de gamestate_keybinds.ini
	std::ifstream ifs("config/gamestate_keybinds.ini");

	if (ifs.is_open()) {
		std::string key = "";
		std::string key2 = "";

		while (ifs >> key >> key2) {
			this->keybinds[key] = this->supportedKeys->at(key2);
		}

	}
	ifs.close();
}

// Constructor y destructor
GameState::GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states, const GameConfig& config)
	: State(window, supportedKeys, states), config(config) {

	this->initKeybinds();
	this->initTextures();
	this->initFonts();
	this->initVariables();
	this->initText();
	this->initPauseMenu();
	this->initBoard(this->textures);
	this->initGamePanel();
}

// Arma una GameConfig básica a partir del modo elegido en el menú: nombres por
// defecto y tiempo/dificultad leídos de config/game.ini (el modo lo fija el botón).
static GameConfig makeMenuConfig(bool aiMode) {
	GameConfig cfg;
	cfg.aiMode = aiMode;
	cfg.playerIsWhite = true;
	cfg.whiteName = "Jugador 1";
	cfg.blackName = aiMode ? "Escafandrin" : "Jugador 2";

	std::ifstream ifs("config/game.ini");
	if (ifs.is_open()) {
		int fileMode = 0, diff = 0;
		ifs >> cfg.baseTime >> cfg.increment;
		if (ifs >> fileMode) { (void)fileMode; } // el modo del .ini se ignora: lo fija el menú
		if (ifs >> diff) cfg.aiDifficulty = diff;
	}
	return cfg;
}

GameState::GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states, bool forceAiMode)
	: GameState(window, supportedKeys, states, makeMenuConfig(forceAiMode))
{
}

GameState::~GameState() {
	this->aiStopFlag = true;
	if (this->aiFutureMove.valid()) {
		this->aiFutureMove.wait();
	}
}



void GameState::updateText() {
	//Actualizamos el texto del marcador
	this->gameInfoText.setString(this->buildScoreText());
}

// Funciones
void GameState::handleEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == this->keybinds.at("CLOSE")) {
			if (!this->paused) {
				this->pauseState();
			}
			else {
				this->unpauseState();
			}
		}
	}

	if (event.type == sf::Event::KeyPressed) {
		// Undo (Ctrl + Z or Left Arrow)
		if ((event.key.code == sf::Keyboard::Z && event.key.control && !event.key.shift) || event.key.code == sf::Keyboard::Left) {
			this->undo();
		}
		// Redo (Ctrl + Y or Ctrl + Shift + Z or Right Arrow)
		if ((event.key.code == sf::Keyboard::Y && event.key.control) || 
			(event.key.code == sf::Keyboard::Z && event.key.control && event.key.shift) ||
			event.key.code == sf::Keyboard::Right) {
			this->redo();
		}
	}

	if (this->moveListPanel) {
		this->moveListPanel->handleEvent(event, *this->window);
	}
}

void GameState::updateInput(float /*dt*/) {
	// Movimiento: admite dos clics (clic en pieza, clic en destino) y arrastrar-soltar.
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	if (!this->paused && !this->board->getEndGame() && !this->board->isPromoting() &&
	    !this->confirmResignActive && !this->drawOfferActive && !this->drawRejectedActive) {
		// Bloquear entrada del usuario si es el turno de la IA
		bool isAITurn = this->aiMode && (this->turn == this->aiPlaysWhite);
		if (isAITurn) return;

		if (mouseDown && !this->mouseHeldLastFrame) {
			// Flanco de pulsación: seleccionar, re-agarrar o soltar destino (dos clics).
			if (!this->board->isAnyPieceAnimating()) {
				this->board->onPress(this->mousePosWindow, this->turn, this->points1, this->points2);
				this->updateText();
			}
		} else if (mouseDown) {
			// Manteniendo pulsado: la pieza agarrada sigue al cursor.
			this->board->onDrag(this->mousePosWindow);
		} else if (this->mouseHeldLastFrame) {
			// Flanco de soltar: completar el arrastre (o cancelarlo si vuelve a su casilla).
			this->board->onRelease(this->mousePosWindow, this->turn, this->points1, this->points2);
			this->updateText();
		}
	}
	this->mouseHeldLastFrame = mouseDown;

	// Botón del mensaje de fin de partida.
	// Exigimos haber soltado el ratón al menos una vez tras el fin de la partida,
	// para que el mismo clic que da el mate no cierre el cuadro al instante.
	if (this->board->getEndGame()) {
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			this->gameOverReady = true;
		}
		if (this->gameOverReady && this->gameOverBox->isButtonPressed()) {
			this->endState();
		}
	}
}

void GameState::updatePauseMenuButtons() {
	// Botón de salir
	if (this->pauseMenu->isButtonPressed("EXIT")) {          
		this->endState();
	}

	// Botón de continuar
	if (this->pauseMenu->isButtonPressed("CONTINUE")) {
		this->unpauseState();
	}

	// Botón de guardar PGN
	if (this->pauseMenu->isButtonPressed("SAVE_PGN")) {
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);
		char dateStr[30];
		std::strftime(dateStr, sizeof(dateStr), "%Y%m%d_%H%M%S", now);
		
		std::string desktopPath = "";
		if (const char* home = std::getenv("HOME")) {
			desktopPath = std::string(home) + "/Desktop/SimplyChess_Partidas/";
		} else {
			desktopPath = "partidas/"; // Fallback
		}
		
		std::filesystem::create_directory(desktopPath);
		std::string filename = desktopPath + "game_" + std::string(dateStr) + ".pgn";
		
		PGNManager::exportToFile(filename,
								 this->board->getMoveHistory(),
								 "White", "Black",
								 this->board->getGameStatus(),
								 this->baseTime, this->increment);
		
		// Unpause after saving or keep it paused, let's keep it paused.
	}
}

void GameState::updateGamePanel() {
	// ── Timer strings ─────────────────────────────────────────────────
	std::string bTime = (this->baseTime > 0.f) ? formatTime(this->timeBlack) : "--:--";
	std::string wTime = (this->baseTime > 0.f) ? formatTime(this->timeWhite) : "--:--";

	blackTimerTxt.setString(bTime);
	whiteTimerTxt.setString(wTime);

	// Recentrar timers (texto de ancho variable)
	constexpr float SX = 822.f, SW = 452.f;
	constexpr float CX = SX + 3.f + 14.f;
	auto rightX = [&](sf::Text& t) {
		auto lb = t.getLocalBounds();
		return SX + SW - 3.f - 14.f - lb.width - lb.left;
	};
	blackTimerTxt.setPosition(rightX(blackTimerTxt), blackTimerTxt.getPosition().y);
	whiteTimerTxt.setPosition(rightX(whiteTimerTxt), whiteTimerTxt.getPosition().y);

	// ── Turn indicator (highlight active player) ─────────────────────
	if (this->turn) {
		// Blancas al turno
		whiteRowBg.setFillColor(sf::Color(200, 168, 90, 26));
		whiteRowBg.setOutlineColor(sf::Color(202, 168, 90, 71));
		turnBar.setFillColor(sf::Color(202, 164, 72));
		whiteStatusTxt.setString("TU TURNO");
		whiteStatusTxt.setFillColor(sf::Color(202, 168, 90, 173));
		whiteTimerTxt.setFillColor(sf::Color(202, 170, 88));

		blackRowBg.setFillColor(sf::Color(20, 10, 4, 71));
		blackRowBg.setOutlineColor(sf::Color(200, 148, 70, 30));
		blackStatusTxt.setString("EN ESPERA");
		blackStatusTxt.setFillColor(sf::Color(200, 160, 100, 107));
		blackTimerTxt.setFillColor(sf::Color(210, 175, 110, 122));
	} else {
		// Negras al turno
		blackRowBg.setFillColor(sf::Color(200, 168, 90, 26));
		blackRowBg.setOutlineColor(sf::Color(202, 168, 90, 71));
		blackStatusTxt.setString("SU TURNO");
		blackStatusTxt.setFillColor(sf::Color(202, 168, 90, 173));
		blackTimerTxt.setFillColor(sf::Color(202, 170, 88));

		whiteRowBg.setFillColor(sf::Color(20, 10, 4, 71));
		whiteRowBg.setOutlineColor(sf::Color(200, 148, 70, 30));
		turnBar.setFillColor(sf::Color::Transparent);
		whiteStatusTxt.setString("EN ESPERA");
		whiteStatusTxt.setFillColor(sf::Color(200, 160, 100, 107));
		whiteTimerTxt.setFillColor(sf::Color(210, 175, 110, 122));
	}

	// ── Timer glow on active player ──────────────────────────────────
	float g = 0.5f + 0.5f * std::sin(glowClock.getElapsedTime().asSeconds() * 3.14159f);
	uint8_t gA = static_cast<uint8_t>(180.f + 75.f * g);
	if (this->turn) {
		sf::Color c = whiteTimerTxt.getFillColor(); c.a = gA;
		whiteTimerTxt.setFillColor(c);
	} else {
		sf::Color c = blackTimerTxt.getFillColor(); c.a = gA;
		blackTimerTxt.setFillColor(c);
	}

	// ── Advantage texts ──────────────────────────────────────────────
	int adv1 = this->points1 - this->points2;
	int adv2 = this->points2 - this->points1;
	blackAdvTxt.setString(adv2 > 0 ? "+" + std::to_string(adv2) : "");
	whiteAdvTxt.setString(adv1 > 0 ? "+" + std::to_string(adv1) : "");

	// ── Action buttons ───────────────────────────────────────────────
	bool isAITurn = this->aiMode && !this->turn;
	if (this->paused || this->board->getEndGame() || this->confirmResignActive || this->drawOfferActive || this->drawRejectedActive || isAITurn) {
		for (auto& btn : actionBtns)
			btn.update({-1000, -1000}); // Desactivar hover
	} else {
		for (auto& btn : actionBtns)
			btn.update(this->mousePosWindow);

		bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		if (!mouseHeldForActionBtns) {
			if (!actionBtns.empty() && actionBtns[0].isPressed()) {
				this->confirmResignActive = true;
			}
			if (actionBtns.size() > 1 && actionBtns[1].isPressed()) {
				if (this->aiMode) {
					GameSnapshot snap = this->board->captureSnapshot();
					FastBoard fastBoard;
					fastBoard.initFromBoardGrid(snap.board, this->turn, snap.castling, snap.peonPaso);
					if (AIEngine::shouldAcceptDraw(fastBoard)) {
						this->board->forceEndGame(GameStatus::DRAW_AGREEMENT);
					} else {
						this->drawRejectedActive = true;
					}
				} else {
					this->drawOfferActive = true;
				}
			}
		}
		mouseHeldForActionBtns = mouseDown;
	}
}

void GameState::launchAsyncEvaluation() {
	if (!this->evalBar || !this->board) return;

	if (this->isEvaluating) {
		this->evalStopFlag = true;
		if (this->evalFuture.valid()) {
			this->evalFuture.wait();
		}
	}
	
	this->evalStopFlag = false;
	
	GameSnapshot snap = this->board->captureSnapshot();
	FastBoard fBoard;
	// Usamos this->turn ya que snapshot puede no tenerlo si no fue un undo state válido
	fBoard.initFromBoardGrid(snap.board, this->turn, snap.castling, snap.peonPaso);
	
	this->evalFuture = std::async(std::launch::async, [fBoard, this]() {
		// Profundidad 3: suficiente para ver intercambios y capturas inminentes.
		return AIEngine::evaluatePosition(fBoard, 3, this->evalStopFlag);
	});
	
	this->isEvaluating = true;
}

void GameState::update(float dt) {

	this->updateMousePositions();

	this->updateKeytime(dt);

	this->updateInput(dt);
	this->updateGamePanel();
	
	if (this->evalBar) {
		this->evalBar->update(dt);
		
		if (this->isEvaluating && this->evalFuture.valid()) {
			if (this->evalFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				int scoreCp = this->evalFuture.get();
				// Convertir centipeones a la escala normal (1.0 = 1 peón)
				float score = scoreCp / 100.0f;
				this->evalBar->setEvaluation(score);
				this->lastEvalScore = score;
				this->isEvaluating = false;
			}
		}
	}

	if (!this->paused) {
		if (this->moveListPanel) {
			this->moveListPanel->update(this->board->getMoveHistory());
		}
		if (!this->board->getEndGame()) {
			// Reloj: no corre hasta que las blancas hacen su primer movimiento.
			// Solo se pausa durante una coronación pendiente: en ese momento el turno
			// ya ha cambiado al rival, así que su reloj no debe correr mientras el
			// jugador elige la pieza. Seleccionar/mover una pieza NO pausa el reloj.
			if (this->baseTime > 0.0f && this->clockStarted && !this->board->isPromoting() &&
			    !this->confirmResignActive && !this->drawOfferActive && !this->drawRejectedActive) {
				if (this->turn) {
					this->timeWhite -= dt;
					if (this->timeWhite <= 0) {
						this->board->forceEndGame(GameStatus::TIMEOUT);
					}
				} else {
					this->timeBlack -= dt;
					if (this->timeBlack <= 0) {
						this->board->forceEndGame(GameStatus::TIMEOUT);
					}
				}
			}

			// Incremento (y arranque del reloj tras el primer movimiento de las blancas)
			if (this->previousTurn != this->turn) {
				if (this->baseTime > 0.0f) {
					// Turno acaba de cambiar
					if (this->previousTurn) {
						this->timeWhite += this->increment;
					} else {
						this->timeBlack += this->increment;
					}
				}
				this->previousTurn = this->turn;
				this->clockStarted = true;
				this->captureStateForUndo();
				
				this->launchAsyncEvaluation();
			}
			this->updateText();
		}

		// Update animations ALWAYS, even if game is over
		this->board->updateAnimations(dt);

		if (this->board->getEndGame()) {
			if (!this->board->isAnyPieceAnimating()) {
				if (!this->gameOverReady) {
					GameStatus status = this->board->getGameStatus();
					if (status == GameStatus::TIMEOUT) {
						if (this->turn) {
							this->gameOverBox->setText("Fin de la partida.\nTiempo agotado. Gana " + this->player2);
						} else {
							this->gameOverBox->setText("Fin de la partida.\nTiempo agotado. Gana " + this->player1);
						}
					} else if (status == GameStatus::CHECKMATE) {
						if (this->turn) {
							this->gameOverBox->setText("Fin de la partida.\nGana " + this->player2);
						} else {
							this->gameOverBox->setText("Fin de la partida.\nGana " + this->player1);
						}
					} else if (status == GameStatus::STALEMATE) {
						this->gameOverBox->setText("Fin de la partida.\nTablas por ahogado");
					} else if (status == GameStatus::FIFTY_MOVE_RULE) {
						this->gameOverBox->setText("Fin de la partida.\nTablas por regla de 50 movimientos");
					} else if (status == GameStatus::REPETITION) {
						this->gameOverBox->setText("Fin de la partida.\nTablas por repeticion");
					} else if (status == GameStatus::RESIGNATION) {
						if (this->turn) {
							this->gameOverBox->setText("Fin de la partida.\nGana " + this->player2 + " por abandono");
						} else {
							this->gameOverBox->setText("Fin de la partida.\nGana " + this->player1 + " por abandono");
						}
					} else if (status == GameStatus::DRAW_AGREEMENT) {
						this->gameOverBox->setText("Fin de la partida.\nTablas por acuerdo");
					}
				} else {
					this->gameOverBox->update(this->mousePosWindow);
				}
			}
		}
		else {
			if (this->confirmResignActive) {
				this->confirmResignBox->update(this->mousePosWindow);
				if (this->confirmResignBox->isButtonPressed()) {
					this->confirmResignActive = false;
					this->board->forceEndGame(GameStatus::RESIGNATION);
				} else if (this->confirmResignBox->isButton2Pressed()) {
					this->confirmResignActive = false;
				}
			} else if (this->drawOfferActive) {
				this->drawOfferBox->update(this->mousePosWindow);
				if (this->drawOfferBox->isButtonPressed()) {
					this->drawOfferActive = false;
					this->board->forceEndGame(GameStatus::DRAW_AGREEMENT);
				} else if (this->drawOfferBox->isButton2Pressed()) {
					this->drawOfferActive = false;
				}
			} else if (this->drawRejectedActive) {
				this->drawRejectedBox->update(this->mousePosWindow);
				if (this->drawRejectedBox->isButtonPressed()) {
					this->drawRejectedActive = false;
				}
			} else {
				this->board->update(this->mousePosWindow, *this->window);

				if (this->aiMode && (this->turn == this->aiPlaysWhite) && !this->board->getEndGame() && !this->board->isAnyPieceAnimating()) {
					if (!this->aiIsThinking) {
						this->startAIThinking();
					} else {
						this->processAIMove();
					}
				}
			}
		}
	}
	else {
		this->pauseMenu->update(this->mousePosWindow);
		this->updatePauseMenuButtons();
	}
	
}

void GameState::render(sf::RenderTarget* target) {
	if (!target) target = this->window;

	// ── Fondo oscuro de madera ──────────────────────────────────────────
	target->draw(this->bgRect);
	target->draw(this->vignetteVA);

	// ── Tablero y piezas ───────────────────────────────────────────────
	this->board->render(*target);

	// Labels tablero
	target->draw(this->dotNegras);
	target->draw(this->labelNegras);
	target->draw(this->dotBlancas);
	target->draw(this->labelBlancas);

	// ── Panel lateral ──────────────────────────────────────────────────
	this->sidePanel.render(*target);

	// Fila jugador negro
	target->draw(this->blackRowBg);
	target->draw(this->blackKingBox);
	target->draw(this->blackKingSprite);
	target->draw(this->blackNameTxt);
	target->draw(this->blackStatusTxt);
	target->draw(this->blackTimerTxt);

	// Separadores y ventajas
	target->draw(this->sepLines[0]);
	target->draw(this->blackAdvTxt);

	// Draw White pieces captured by Black (y = 92.f)
	{
		std::vector<Piece*> capturedWhite = this->board->getCapturedPieces(true);
		std::sort(capturedWhite.begin(), capturedWhite.end(), [](const Piece* a, const Piece* b) {
			if (a->getPoints() != b->getPoints()) return a->getPoints() < b->getPoints();
			return static_cast<int>(a->getType()) < static_cast<int>(b->getType());
		});
		sf::Sprite capturedWhiteSprite;
		capturedWhiteSprite.setScale(0.24f, 0.24f);
		float startX = 839.f;
		for (size_t i = 0; i < capturedWhite.size(); ++i) {
			std::string textureKey = Board::getTextureKey(capturedWhite[i]->getType(), true);
			capturedWhiteSprite.setTexture(this->textures[textureKey]);
			capturedWhiteSprite.setPosition(startX + i * 26.f, 92.f);
			target->draw(capturedWhiteSprite);
		}
	}

	target->draw(this->sepLines[1]);

	// Lista de movimientos
	if (this->moveListPanel)
		this->moveListPanel->render(*target);

	// EvalBar
	if (this->evalBar)
		this->evalBar->render(*target);

	target->draw(this->sepLines[2]);
	target->draw(this->whiteAdvTxt);

	// Draw Black pieces captured by White (y = 649.f)
	{
		std::vector<Piece*> capturedBlack = this->board->getCapturedPieces(false);
		std::sort(capturedBlack.begin(), capturedBlack.end(), [](const Piece* a, const Piece* b) {
			if (a->getPoints() != b->getPoints()) return a->getPoints() < b->getPoints();
			return static_cast<int>(a->getType()) < static_cast<int>(b->getType());
		});
		sf::Sprite capturedBlackSprite;
		capturedBlackSprite.setScale(0.24f, 0.24f);
		float startX = 839.f;
		for (size_t i = 0; i < capturedBlack.size(); ++i) {
			std::string textureKey = Board::getTextureKey(capturedBlack[i]->getType(), false);
			capturedBlackSprite.setTexture(this->textures[textureKey]);
			capturedBlackSprite.setPosition(startX + i * 26.f, 649.f);
			target->draw(capturedBlackSprite);
		}
	}

	target->draw(this->sepLines[3]);

	// Fila jugador blanco
	target->draw(this->whiteRowBg);
	target->draw(this->turnBar);
	target->draw(this->whiteKingBox);
	target->draw(this->whiteKingSprite);
	target->draw(this->whiteNameTxt);
	target->draw(this->whiteStatusTxt);
	target->draw(this->whiteTimerTxt);

	target->draw(this->sepLines[4]);

	// Botones de acción
	for (auto& btn : this->actionBtns)
		btn.render(*target);

	// Game over y pausa
	if (this->board->getEndGame())
		this->gameOverBox->render(*target);

	if (this->paused)
		this->pauseMenu->render(*target);

	if (this->confirmResignActive)
		this->confirmResignBox->render(*target);

	if (this->drawOfferActive)
		this->drawOfferBox->render(*target);

	if (this->drawRejectedActive)
		this->drawRejectedBox->render(*target);
}

void GameState::captureStateForUndo() {
	if (this->baseTime > 0.0f) return; // Undo/Redo is disabled when clock is active

	if (this->currentMoveIndex < (int)this->undoStack.size() - 1) {
		this->undoStack.erase(this->undoStack.begin() + this->currentMoveIndex + 1, this->undoStack.end());
	}
	
	GameSnapshot snap = this->board->captureSnapshot();
	snap.turn = this->turn;
	snap.points1 = this->points1;
	snap.points2 = this->points2;
	snap.timeWhite = this->timeWhite;
	snap.timeBlack = this->timeBlack;
	
	this->undoStack.push_back(snap);
	this->currentMoveIndex++;
}

void GameState::undo() {
	if (this->baseTime > 0.0f || this->aiIsThinking) return;
	
	int steps = this->aiMode ? 2 : 1;
	bool undone = false;
	for (int i = 0; i < steps; ++i) {
		if (this->currentMoveIndex > 0) {
			this->currentMoveIndex--;
			undone = true;
		}
	}
	
	if (undone) {
		const GameSnapshot& snap = this->undoStack[this->currentMoveIndex];
		this->board->restoreSnapshot(snap, this->textures);
		this->turn = snap.turn;
		this->points1 = snap.points1;
		this->points2 = snap.points2;
		this->timeWhite = snap.timeWhite;
		this->timeBlack = snap.timeBlack;
		this->previousTurn = this->turn;
		this->updateText();
		this->launchAsyncEvaluation();
	}
}

void GameState::redo() {
	if (this->baseTime > 0.0f || this->aiIsThinking) return;
	
	int steps = this->aiMode ? 2 : 1;
	bool redone = false;
	for (int i = 0; i < steps; ++i) {
		if (this->currentMoveIndex < (int)this->undoStack.size() - 1) {
			this->currentMoveIndex++;
			redone = true;
		}
	}
	
	if (redone) {
		const GameSnapshot& snap = this->undoStack[this->currentMoveIndex];
		this->board->restoreSnapshot(snap, this->textures);
		this->turn = snap.turn;
		this->points1 = snap.points1;
		this->points2 = snap.points2;
		this->timeWhite = snap.timeWhite;
		this->timeBlack = snap.timeBlack;
		this->previousTurn = this->turn;
		this->updateText();
		this->launchAsyncEvaluation();
	}
}

void GameState::startAIThinking() {
	this->aiIsThinking = true;
	this->aiStopFlag = false;
	
	GameSnapshot snap = this->board->captureSnapshot();
	FastBoard fastBoard;
	// captureSnapshot() no rellena snap.turn (es estado de GameState), así que
	// usamos el turno actual autoritativo en lugar de un valor indeterminado.
	fastBoard.initFromBoardGrid(snap.board, this->turn, snap.castling, snap.peonPaso);

	this->aiFutureMove = std::async(std::launch::async, [fastBoard, this]() {
		return AIEngine::getBestMove(fastBoard, this->aiDifficulty, this->aiStopFlag);
	});
}

void GameState::processAIMove() {
	if (this->aiFutureMove.valid() && this->aiFutureMove.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
		FastMove move = this->aiFutureMove.get();
		this->aiIsThinking = false;
		
		if (move.movedPiece != FAST_EMPTY) {
			PieceType promo = PieceType::REINA;
			if (move.isPromotion) {
				switch (move.promotionType) {
					case FAST_QUEEN: promo = PieceType::REINA; break;
					case FAST_ROOK: promo = PieceType::TORRE; break;
					case FAST_BISHOP: promo = PieceType::ALFIL; break;
					case FAST_KNIGHT: promo = PieceType::CABALLO; break;
				}
			}
			
			bool turnBefore = this->turn;
			this->board->applyAIMove(move.fromY, move.fromX, move.toY, move.toX, promo, this->turn, this->points1, this->points2);
			
			if (this->turn == turnBefore) {
				// Fallback de seguridad: la jugada elegida fue rechazada (ilegal).
				// Escaneamos el tablero real por la primera jugada legal para evitar que el juego se congele.
				bool foundBackup = false;
				for (int fx = 0; fx < 8; ++fx) {
					for (int fy = 0; fy < 8; ++fy) {
						Piece* p = this->board->getPiece(fx, fy);
						if (p && !p->getColor() && p->isActive()) { // Pieza negra activa de la IA
							sf::Vector2i startPos(fx, fy);
							for (int tx = 0; tx < 8; ++tx) {
								for (int ty = 0; ty < 8; ++ty) {
									sf::Vector2i desPos(tx, ty);
									if (startPos == desPos) continue;
									
									GameSnapshot snap = this->board->captureSnapshot();
									BoardGrid testBoard = snap.board;
									CastlingState testCastling = snap.castling;
									EnPassantState testPeonPaso = snap.peonPaso;
									
									Piece* target = this->board->getPiece(tx, ty);
									if (this->board->checkMove(false, startPos, desPos, p, target, testCastling, testBoard, testPeonPaso)) {
										testBoard[desPos.x][desPos.y] = testBoard[startPos.x][startPos.y];
										if ((startPos.x + startPos.y) % 2 != 0) testBoard[startPos.x][startPos.y] = "-";
										else testBoard[startPos.x][startPos.y] = "+";
										
										if (!this->board->isInCheck(false, testBoard)) {
											this->board->applyAIMove(startPos.x, startPos.y, desPos.x, desPos.y, PieceType::REINA, this->turn, this->points1, this->points2);
											foundBackup = true;
											break;
										}
									}
								}
								if (foundBackup) break;
							}
						}
						if (foundBackup) break;
					}
					if (foundBackup) break;
				}
				
				if (!foundBackup) {
					if (this->board->isInCheck(false)) {
						this->board->forceEndGame(GameStatus::CHECKMATE);
					} else {
						this->board->forceEndGame(GameStatus::STALEMATE);
					}
				}
			}
		} else {
			// Fallback: si la IA no encuentra movimientos legales (jaque mate o ahogado)
			if (this->board->isInCheck(false)) {
				this->board->forceEndGame(GameStatus::CHECKMATE);
			} else {
				this->board->forceEndGame(GameStatus::STALEMATE);
			}
		}
	}
}
