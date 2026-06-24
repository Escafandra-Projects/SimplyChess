#include "states/GameSetupState.h"
#include "states/GameState.h"

#include <stdexcept>
#include <cstdlib>
#include <ctime>

// Inicialización
void GameSetupState::initVariables() {
	this->currentMode = 1;  // Por defecto: IA (se sobrescribe con loadDefaults / forcedMode)
	this->currentColor = 0; // Por defecto: Blancas
	this->mousePressedLastFrame = false;
	this->keytimeMax = 30.0f;

	this->baseTime = 300.f;
	this->increment = 0.f;
	this->aiDifficulty = 4;
}

void GameSetupState::initKeybinds() {
}

void GameSetupState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Gameplay.ttf")) {
		throw std::runtime_error("ERROR::GAME_SETUP_STATE::COULD NOT LOAD FONT");
	}
}

void GameSetupState::initTextures() {
	// Fondo oscuro + panel de madera (mismo tema que Ajustes).
	this->bgRect.setSize({1280.f, 820.f});
	this->bgRect.setFillColor(sf::Color(26, 12, 6));
	this->panel.setBounds({190.f, 60.f, 900.f, 700.f});
}

void GameSetupState::initText() {
	this->titleText.setFont(this->font);
	this->titleText.setCharacterSize(44);
	this->titleText.setLetterSpacing(2.5f);
	this->titleText.setString("NUEVA PARTIDA");
	this->titleText.setFillColor(sf::Color(242, 226, 192));
	auto tb = this->titleText.getLocalBounds();
	this->titleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
	this->titleText.setPosition(1280.f / 2.f, 115.f);

	auto makeLabel = [&](sf::Text& t, const std::string& s, float y) {
		t.setFont(this->font);
		t.setCharacterSize(28);
		t.setString(s);
		t.setFillColor(sf::Color(238, 224, 194));
		t.setPosition(350.f, y);
	};
	makeLabel(this->modeLabel,  "Rival:",      250.f);
	makeLabel(this->nameLabel,  "Tu nombre:",  360.f);
	makeLabel(this->colorLabel, "Juegas:",     470.f);
	makeLabel(this->whiteLabel, "Blancas:",    360.f);
	makeLabel(this->blackLabel, "Negras:",     470.f);
}

void GameSetupState::initButtons() {
	this->buttons["MODE_CYCLE"]  = std::make_unique<MenuButton>(650.f, 240.f, 250.f, 50.f, &this->font, "Escafandrin", 24);
	this->buttons["COLOR_CYCLE"] = std::make_unique<MenuButton>(650.f, 460.f, 250.f, 50.f, &this->font, "Blancas", 24);
	this->buttons["BACK"]        = std::make_unique<MenuButton>(350.f, 580.f, 200.f, 50.f, &this->font, "Atras", 28);
	this->buttons["START"]       = std::make_unique<MenuButton>(650.f, 580.f, 200.f, 50.f, &this->font, "Empezar", 28);
}

void GameSetupState::initFields() {
	this->field1 = std::make_unique<TextField>(650.f, 355.f, 250.f, 50.f, &this->font, 24, "Jugador 1");
	this->field2 = std::make_unique<TextField>(650.f, 465.f, 250.f, 50.f, &this->font, 24, "Jugador 2");
	this->focusField(this->field1.get());
}

void GameSetupState::loadDefaults() {
	std::ifstream ifs("config/game.ini");
	if (ifs.is_open()) {
		float bt = 300.f, inc = 0.f;
		int mode = 1, diff = 4;
		ifs >> bt >> inc;
		if (!(ifs >> mode)) mode = 1;
		if (!(ifs >> diff)) diff = 4;
		ifs.close();

		this->baseTime = bt;
		this->increment = inc;
		this->aiDifficulty = diff;
		this->currentMode = (mode != 0) ? 1 : 0;
	}
}

void GameSetupState::updateButtonTexts() {
	this->buttons["MODE_CYCLE"]->setText(this->currentMode == 1 ? "Escafandrin" : "Local 2P");

	const char* colorNames[3] = { "Blancas", "Negras", "Aleatorio" };
	this->buttons["COLOR_CYCLE"]->setText(colorNames[this->currentColor]);
}

void GameSetupState::focusField(TextField* target) {
	if (this->field1) this->field1->setFocused(this->field1.get() == target);
	if (this->field2) this->field2->setFocused(this->field2.get() == target);
}

// Constructor y destructor
GameSetupState::GameSetupState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states, int forcedMode)
	: State(window, supportedKeys, states) {
	this->initVariables();
	this->loadDefaults();
	// El menú puede preseleccionar el rival (vs IA / 2 jugadores); si no, vale el de game.ini.
	if (forcedMode == 0 || forcedMode == 1) this->currentMode = forcedMode;
	this->initFonts();
	this->initTextures();
	this->initText();
	this->initButtons();
	this->initFields();
	this->updateButtonTexts();

	std::srand(static_cast<unsigned>(std::time(nullptr)));
}

GameSetupState::~GameSetupState() {
}

void GameSetupState::startGame() {
	GameConfig config;
	config.aiMode = (this->currentMode == 1);
	config.aiDifficulty = this->aiDifficulty;
	config.baseTime = this->baseTime;
	config.increment = this->increment;

	// Nombre por defecto si el campo se deja vacío.
	auto orDefault = [](const std::string& s, const std::string& def) {
		return s.empty() ? def : s;
	};

	if (config.aiMode) {
		// Resolver el color (aleatorio se decide ahora).
		bool playerWhite = (this->currentColor == 0);
		if (this->currentColor == 2) playerWhite = (std::rand() % 2) == 0;
		config.playerIsWhite = playerWhite;

		std::string playerName = orDefault(this->field1->getText(), "Jugador");
		if (playerWhite) {
			config.whiteName = playerName;
			config.blackName = "Escafandrin";
		} else {
			config.whiteName = "Escafandrin";
			config.blackName = playerName;
		}
	} else {
		config.playerIsWhite = true; // 2 jugadores: tablero siempre con blancas abajo
		config.whiteName = orDefault(this->field1->getText(), "Blancas");
		config.blackName = orDefault(this->field2->getText(), "Negras");
	}

	// Lanza la partida y marca este estado para retirarse: al terminar la
	// partida se vuelve directamente al menú principal.
	this->states->push(std::make_unique<GameState>(this->window, this->supportedKeys, this->states, config));
	this->quit = true;
}

void GameSetupState::handleEvent(const sf::Event& event) {
	if (this->field1) this->field1->handleEvent(event);
	if (this->field2) this->field2->handleEvent(event);
}

void GameSetupState::updateInput(float /*dt*/) {
}

void GameSetupState::updateButtons() {
	bool isAI = (this->currentMode == 1);

	this->buttons["MODE_CYCLE"]->update(this->mousePosWindow);
	this->buttons["BACK"]->update(this->mousePosWindow);
	this->buttons["START"]->update(this->mousePosWindow);
	if (isAI) this->buttons["COLOR_CYCLE"]->update(this->mousePosWindow);

	if (this->buttons["MODE_CYCLE"]->isPressed() && this->getKeytime()) {
		this->currentMode = (this->currentMode + 1) % 2;
		this->updateButtonTexts();
	}
	if (isAI && this->buttons["COLOR_CYCLE"]->isPressed() && this->getKeytime()) {
		this->currentColor = (this->currentColor + 1) % 3;
		this->updateButtonTexts();
	}
	if (this->buttons["BACK"]->isPressed() && this->getKeytime()) {
		this->quit = true;
	}
	if (this->buttons["START"]->isPressed() && this->getKeytime()) {
		this->startGame();
	}
}

void GameSetupState::update(float dt) {
	this->updateMousePositions();
	this->updateKeytime(dt);

	// Enfoque de los campos al hacer clic (flanco de pulsación).
	bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	if (mousePressed && !this->mousePressedLastFrame) {
		bool isAI = (this->currentMode == 1);
		if (this->field1->isMouseOver(this->mousePosWindow)) {
			this->focusField(this->field1.get());
		} else if (!isAI && this->field2->isMouseOver(this->mousePosWindow)) {
			this->focusField(this->field2.get());
		}
	}

	this->updateButtons();

	this->field1->update(dt);
	this->field2->update(dt);

	this->mousePressedLastFrame = mousePressed;
}

void GameSetupState::render(sf::RenderTarget* target) {
	if (!target) {
		target = this->window;
	}

	target->draw(this->bgRect);
	this->panel.render(*target);
	target->draw(this->titleText);
	target->draw(this->modeLabel);

	this->buttons["MODE_CYCLE"]->render(*target);
	this->buttons["BACK"]->render(*target);
	this->buttons["START"]->render(*target);

	if (this->currentMode == 1) { // IA
		target->draw(this->nameLabel);
		target->draw(this->colorLabel);
		this->field1->render(*target);
		this->buttons["COLOR_CYCLE"]->render(*target);
	} else { // Local 2 jugadores
		target->draw(this->whiteLabel);
		target->draw(this->blackLabel);
		this->field1->render(*target);
		this->field2->render(*target);
	}
}
