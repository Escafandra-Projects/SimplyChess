#include "states/MainMenuState.h"
#include "states/GameSetupState.h"
#include "states/SettingsState.h"

#include <stdexcept>


// Inicialización
void MainMenuState::initVariables()
{
	this->titleText.setFont(this->font);
	this->titleText.setCharacterSize(100);
	//this->titleText.setColor(sf::Color::White);
	this->titleText.setString("Simply Chess");
	this->titleText.setPosition(380.f, 130.f);

	this->mousePressedLastFrame = false;
}

void MainMenuState::initTextures()
{
	// Fondo
	if (!this->textures["BACKGROUND"].loadFromFile("resources/images/menu/background.png")) {
		throw std::runtime_error("ERROR::MAIN_MENU_STATE::FAILED TO LOAD BACKGROUND");
	}
	this->background.setTexture(this->textures["BACKGROUND"]);
	this->background.scale(5.12f, 4.1f);

	// Botones
	if (!this->textures["BUTTONS"].loadFromFile("resources/images/interface/buttons.png")) {
		throw std::runtime_error("ERROR::MAIN_MENU_STATE::FAILED TO LOAD BUTTONS");
	}
}

void MainMenuState::initKeybinds() {
	// Obtiene keybinds de gamestate_keybinds.ini
	std::ifstream ifs("config/mainmenustate_keybinds.ini");

	if (ifs.is_open()) {
		std::string key = "";
		std::string key2 = "";

		while (ifs >> key >> key2) {
			this->keybinds[key] = this->supportedKeys->at(key2);
		}

	}
	ifs.close();
}

void MainMenuState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Factory LJDS.ttf")) {
		throw std::runtime_error("ERROR::MAIN_MENU_STATE::COULD NOT LOAD FONT");
	}
}

void MainMenuState::initButtons() {
	this->buttons["GAME_STATE"] = std::make_unique<Button>(495.0f, 550.0f, 100.f, 61.0f, 
		&this->font, "Start", 50,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["GAME_STATE"]->scale(3, 3);
	


	this->buttons["SETTINGS_STATE"] = std::make_unique<Button>(180.0f, 400.0f, 100.f, 61.0f,
		&this->font, "Settings", 50,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["SETTINGS_STATE"]->scale(3, 3);
		


	this->buttons["EXIT_STATE"] = std::make_unique<Button>(820.0f, 400.0f, 100.0f, 61.0f,
		&this->font, "Exit", 50,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["EXIT_STATE"]->scale(3, 3);
		
}

// Constructor y destructor
MainMenuState::MainMenuState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states) : State(window, supportedKeys, states)
{
	this->initVariables();
	this->initTextures();
	this->initFonts();
	this->initKeybinds();
	this->initButtons();
}

MainMenuState::~MainMenuState() {
}

// Funciones
void MainMenuState::updateInput(float /*dt*/) {

}

void MainMenuState::updateButtons() {

	// Actualiza todos los botones y su funcionalidad
	for (auto& [name, button] : this->buttons) {
		button->update(this->mousePosWindow);
	}

	// Solo procesamos un clic nuevo (flanco de pulsación). De lo contrario, un clic
	// mantenido proveniente de otro estado (p. ej. el botón Back de Settings, que se
	// solapa con Start) se "colaría" y lanzaría un botón nada más volver al menú.
	bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	if (!this->mousePressedLastFrame) {
		// Nuevo juego: primero la pantalla de configuración (nombres, rival, color).
		if (this->buttons["GAME_STATE"]->isPressed()) {
			this->states->push(std::make_unique<GameSetupState>(this->window, this->supportedKeys, this->states));
		}
		// Ajustes
		else if (this->buttons["SETTINGS_STATE"]->isPressed()) {
			this->states->push(std::make_unique<SettingsState>(this->window, this->supportedKeys, this->states));
		}
		// Salir del juego
		else if (this->buttons["EXIT_STATE"]->isPressed()) {
			this->quit = true;
		}
	}

	this->mousePressedLastFrame = mousePressed;
}

void MainMenuState::update(float dt) {
	this->updateInput(dt);
	this->updateMousePositions();
	this->updateButtons();
}

void MainMenuState::renderButtons(sf::RenderTarget& target) {
	for (auto& [name, button] : this->buttons) {
		button->render(target);
	}
}

void MainMenuState::render(sf::RenderTarget* target) {
	// Si el target esta por defecto, utilizamos la ventana
	if (!target) {
		target = this->window;
	}
	target->draw(background);

	target->draw(this->titleText);

	this->renderButtons(*target);
}