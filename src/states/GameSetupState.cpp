#include "states/GameSetupState.h"
#include "states/GameState.h"

#include <stdexcept>
#include <cstdlib>
#include <ctime>

// Inicialización
void GameSetupState::initVariables() {
	this->currentMode = 1;  // Por defecto: IA (se sobrescribe con loadDefaults)
	this->currentColor = 0; // Por defecto: Blancas
	this->mousePressedLastFrame = false;
	this->keytimeMax = 20.0f;

	this->baseTime = 300.f;
	this->increment = 0.f;
	this->aiDifficulty = 4;
}

void GameSetupState::initKeybinds() {
}

void GameSetupState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Factory LJDS.ttf")) {
		throw std::runtime_error("ERROR::GAME_SETUP_STATE::COULD NOT LOAD FONT");
	}
}

void GameSetupState::initTextures() {
	if (!this->textures["BACKGROUND"].loadFromFile("resources/images/menu/background.png")) {
		throw std::runtime_error("ERROR::GAME_SETUP_STATE::FAILED TO LOAD BACKGROUND");
	}
	this->background.setTexture(this->textures["BACKGROUND"]);
	this->background.scale(5.12f, 4.1f);

	if (!this->textures["BUTTONS"].loadFromFile("resources/images/interface/buttons.png")) {
		throw std::runtime_error("ERROR::GAME_SETUP_STATE::FAILED TO LOAD BUTTONS");
	}
}

void GameSetupState::initText() {
	this->titleText.setFont(this->font);
	this->titleText.setCharacterSize(80);
	this->titleText.setString("New Game");
	// Centrado horizontal sobre el ancho de la ventana (evita posiciones descuadradas).
	sf::FloatRect titleBounds = this->titleText.getLocalBounds();
	float winW = static_cast<float>(this->window->getSize().x);
	this->titleText.setPosition((winW - titleBounds.width) / 2.f - titleBounds.left, 140.f);

	this->modeLabel.setFont(this->font);
	this->modeLabel.setCharacterSize(40);
	this->modeLabel.setString("Opponent:");
	this->modeLabel.setPosition(300.f, 295.f);

	this->nameLabel.setFont(this->font);
	this->nameLabel.setCharacterSize(40);
	this->nameLabel.setString("Your name:");
	this->nameLabel.setPosition(300.f, 410.f);

	this->colorLabel.setFont(this->font);
	this->colorLabel.setCharacterSize(40);
	this->colorLabel.setString("You play:");
	this->colorLabel.setPosition(300.f, 515.f);

	this->whiteLabel.setFont(this->font);
	this->whiteLabel.setCharacterSize(40);
	this->whiteLabel.setString("White:");
	this->whiteLabel.setPosition(300.f, 410.f);

	this->blackLabel.setFont(this->font);
	this->blackLabel.setCharacterSize(40);
	this->blackLabel.setString("Black:");
	this->blackLabel.setPosition(300.f, 515.f);
}

void GameSetupState::initButtons() {
	this->buttons["MODE_CYCLE"] = std::make_unique<Button>(630.0f, 290.0f, 100.f, 61.0f,
		&this->font, "Escafandrin", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["MODE_CYCLE"]->scale(2.8f, 2.5f);

	this->buttons["COLOR_CYCLE"] = std::make_unique<Button>(630.0f, 505.0f, 100.f, 61.0f,
		&this->font, "White", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["COLOR_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["BACK"] = std::make_unique<Button>(300.0f, 640.0f, 100.0f, 61.0f,
		&this->font, "Back", 40,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["BACK"]->scale(2.5f, 2.5f);

	this->buttons["START"] = std::make_unique<Button>(770.0f, 640.0f, 100.0f, 61.0f,
		&this->font, "Start", 40,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["START"]->scale(3.0f, 3.0f);
}

void GameSetupState::initFields() {
	this->field1 = std::make_unique<TextField>(600.f, 405.f, 380.f, 60.f, &this->font, 35, "Player 1");
	this->field2 = std::make_unique<TextField>(600.f, 510.f, 380.f, 60.f, &this->font, 35, "Player 2");
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

	const char* colorNames[3] = { "White", "Black", "Random" };
	this->buttons["COLOR_CYCLE"]->setText(colorNames[this->currentColor]);
}

void GameSetupState::focusField(TextField* target) {
	if (this->field1) this->field1->setFocused(this->field1.get() == target);
	if (this->field2) this->field2->setFocused(this->field2.get() == target);
}

// Constructor y destructor
GameSetupState::GameSetupState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states)
	: State(window, supportedKeys, states) {
	this->initVariables();
	this->loadDefaults();
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

		std::string playerName = orDefault(this->field1->getText(), "Player");
		if (playerWhite) {
			config.whiteName = playerName;
			config.blackName = "Escafandrin";
		} else {
			config.whiteName = "Escafandrin";
			config.blackName = playerName;
		}
	} else {
		config.playerIsWhite = true; // 2 jugadores: tablero siempre con blancas abajo
		config.whiteName = orDefault(this->field1->getText(), "White");
		config.blackName = orDefault(this->field2->getText(), "Black");
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

	// Actualizar botones visibles.
	this->buttons["MODE_CYCLE"]->update(this->mousePosWindow);
	this->buttons["BACK"]->update(this->mousePosWindow);
	this->buttons["START"]->update(this->mousePosWindow);
	if (isAI) this->buttons["COLOR_CYCLE"]->update(this->mousePosWindow);

	// Ciclos con antirrebote.
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

	target->draw(this->background);
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
