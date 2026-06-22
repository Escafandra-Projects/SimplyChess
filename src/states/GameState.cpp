#include "states/GameState.h"
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include "chess/PGNManager.h"

#include <stdexcept>

// Inicialización
void GameState::initVariables() {
	this->paused = false;
	this->turn = true;
	this->previousTurn = true;

	this->player1 = "Player 1";
	this->player2 = "Player 2";
	this->points1 = 0;
	this->points2 = 0;
	this->points2 = 0;
	this->gameOverReady = false;
	this->mouseHeldLastFrame = false;
	this->mouseHeldForButtons = false;
	this->background.setPosition(sf::Vector2f(820.f, 0.f));

	std::ifstream ifs("config/game.ini");
	if (ifs.is_open()) {
		ifs >> this->baseTime >> this->increment;
		if (!(ifs >> this->aiMode)) this->aiMode = true;
		if (!(ifs >> this->aiDifficulty)) this->aiDifficulty = 4;
		ifs.close();
	} else {
		this->baseTime = 300.0f;
		this->increment = 0.0f;
		this->aiMode = true;
		this->aiDifficulty = 4;
	}
	this->timeWhite = this->baseTime;
	this->timeBlack = this->baseTime;
	this->clockStarted = false;

	this->aiIsThinking = false;
	this->aiStopFlag = false;

	// Mensaje fin del juego
	this->gameOverBox = std::make_unique<MessageBox>(font, "Game over", "Exit", this->textures["BUTTONS"] );
}   
   

void GameState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Factory LJDS.ttf")) {
		throw std::runtime_error("ERROR::GAME_STATE::COULD NOT LOAD FONT");
	}
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
	this->pauseMenu = std::make_unique<PauseMenu>(this->font);
	this->pauseMenu->addButton("EXIT", 50.f, 370.f, "Exit", this->textures["BUTTONS"]);
	this->pauseMenu->addButton("SAVE_PGN", 50.f, 270.f, "Save PGN", this->textures["BUTTONS"]);
	this->pauseMenu->addButton("CONTINUE", 50.f, 170.f, "Continue", this->textures["BUTTONS"]);
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

	this->moveListPanel = std::make_unique<MoveListPanel>(this->font, sf::FloatRect(850.f, 400.f, 380.f, 340.f));
	
	this->btnUndo = std::make_unique<Button>(850.f, 750.f, 100.f, 61.0f,
		&this->font, "Undo", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	
	this->btnRedo = std::make_unique<Button>(1130.f, 750.f, 100.f, 61.0f,
		&this->font, "Redo", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
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
GameState::GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states) : State(window, supportedKeys, states) {

	this->initKeybinds();
	this->initTextures();
	this->initFonts();
	this->initVariables();
	this->initText();
	this->initPauseMenu();
	this->initBoard(this->textures);
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
	if (!this->paused && !this->board->getEndGame() && !this->board->isPromoting()) {
		// Bloquear entrada del usuario si es el turno de la IA
		bool isAITurn = this->aiMode && !this->turn;
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

void GameState::update(float dt) {

	this->updateMousePositions();
	
	this->updateKeytime(dt);

	this->updateInput(dt);
	
	if (!this->paused) {
		if (this->moveListPanel) {
			this->moveListPanel->update(this->board->getMoveHistory());
		}
		if (!this->board->getEndGame()) {
			// Reloj: no corre hasta que las blancas hacen su primer movimiento.
			// Solo se pausa durante una coronación pendiente: en ese momento el turno
			// ya ha cambiado al rival, así que su reloj no debe correr mientras el
			// jugador elige la pieza. Seleccionar/mover una pieza NO pausa el reloj.
			if (this->baseTime > 0.0f && this->clockStarted && !this->board->isPromoting()) {
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
			}
			this->updateText();
			
			if (this->btnUndo) {
			    this->btnUndo->update(this->mousePosWindow);
			    if (this->btnUndo->isPressed() && !this->mouseHeldForButtons) {
			        this->undo();
			    }
			}
			if (this->btnRedo) {
			    this->btnRedo->update(this->mousePosWindow);
			    if (this->btnRedo->isPressed() && !this->mouseHeldForButtons) {
			        this->redo();
			    }
			}
			this->mouseHeldForButtons = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		}

		// Update animations ALWAYS, even if game is over
		this->board->updateAnimations(dt);

		if (this->board->getEndGame()) {
			if (!this->board->isAnyPieceAnimating()) {
				if (!this->gameOverReady) {
					GameStatus status = this->board->getGameStatus();
					if (status == GameStatus::TIMEOUT) {
						if (this->turn) {
							this->gameOverBox->setText("Game over.\nTime Out. " + this->player2 + " wins");
						} else {
							this->gameOverBox->setText("Game over.\nTime Out. " + this->player1 + " wins");
						}
					} else if (status == GameStatus::CHECKMATE) {
						if (this->turn) {
							this->gameOverBox->setText("Game over.\n" + this->player2 + " wins");
						} else {
							this->gameOverBox->setText("Game over.\n" + this->player1 + " wins");
						}
					} else if (status == GameStatus::STALEMATE) {
						this->gameOverBox->setText("Game over.\nDraw by stalemate");
					} else if (status == GameStatus::FIFTY_MOVE_RULE) {
						this->gameOverBox->setText("Game over.\nDraw by 50-move rule");
					} else if (status == GameStatus::REPETITION) {
						this->gameOverBox->setText("Game over.\nDraw by repetition");
					}
					this->gameOverReady = true;
				} else {
					this->gameOverBox->update(this->mousePosWindow);
				}
			}
		}
		else {
			this->board->update(this->mousePosWindow, *this->window);
			
			if (this->aiMode && !this->turn && !this->board->getEndGame() && !this->board->isAnyPieceAnimating()) {
				if (!this->aiIsThinking) {
					this->startAIThinking();
				} else {
					this->processAIMove();
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
	// Si no se pasa target, se usa la ventana
	if (!target) {
		target = this->window;
	}

	// Tablero y piezas
	this->board->render(*target);

	// Interfaz
	target->draw(this->background);
	target->draw(this->gameInfoText);

	if (this->moveListPanel) {
		this->moveListPanel->render(*target);
	}
	
	if (this->baseTime == 0.0f) { // Only render if undo/redo is active (clock disabled)
	    if (this->btnUndo) this->btnUndo->render(*target);
	    if (this->btnRedo) this->btnRedo->render(*target);
	}

	if (this->board->getEndGame()) {
		this->gameOverBox->render(*target);
	}

	if (this->paused) {
		this->pauseMenu->render(*target);
	}

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
	if (this->baseTime > 0.0f) return;
	if (this->currentMoveIndex > 0) {
		this->currentMoveIndex--;
		const GameSnapshot& snap = this->undoStack[this->currentMoveIndex];
		this->board->restoreSnapshot(snap, this->textures);
		this->turn = snap.turn;
		this->points1 = snap.points1;
		this->points2 = snap.points2;
		this->timeWhite = snap.timeWhite;
		this->timeBlack = snap.timeBlack;
		this->previousTurn = this->turn;
		this->updateText();
	}
}

void GameState::redo() {
	if (this->baseTime > 0.0f) return;
	if (this->currentMoveIndex < (int)this->undoStack.size() - 1) {
		this->currentMoveIndex++;
		const GameSnapshot& snap = this->undoStack[this->currentMoveIndex];
		this->board->restoreSnapshot(snap, this->textures);
		this->turn = snap.turn;
		this->points1 = snap.points1;
		this->points2 = snap.points2;
		this->timeWhite = snap.timeWhite;
		this->timeBlack = snap.timeBlack;
		this->previousTurn = this->turn;
		this->updateText();
	}
}

void GameState::startAIThinking() {
	this->aiIsThinking = true;
	this->aiStopFlag = false;
	
	GameSnapshot snap = this->board->captureSnapshot();
	FastBoard fastBoard;
	fastBoard.initFromBoardGrid(snap.board, snap.turn, snap.castling, snap.peonPaso);
	
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
			this->board->applyAIMove(move.fromY, move.fromX, move.toY, move.toX, promo, this->turn, this->points1, this->points2);
		}
	}
}
