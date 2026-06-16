#include "core/Game.h"
#include "states/GameState.h"
#include "states/MainMenuState.h"



// Inicialización
void Game::initVariables() {
	     
	this->fullscreen = false;
	this->dt = 0.0f;
}

void Game::initWindow() {
	// Crea una ventana SFML según la configuración
	std::ifstream ifs("config/window.ini");
	this->videoModes = sf::VideoMode::getFullscreenModes();

	// Ajustes por defecto
	std::string title = "None";
	sf::VideoMode windowBounds = sf::VideoMode::getDesktopMode();
	unsigned framerateLimit = 120;
	bool verticalSyncEnabled = false;
	unsigned antialiasingLevel = 0;

	if (ifs.is_open()) {
		std::getline(ifs, title);
		ifs >> windowBounds.width >> windowBounds.height;
		ifs >> fullscreen;
		ifs >> framerateLimit;
		ifs >> verticalSyncEnabled;
		ifs >> antialiasingLevel;
	}
	else {
		std::cout << "Error al abrir el archivo window.ini, configuración establecida por defecto" << std::endl;
	}
	ifs.close();

	// Una vez leida la configuración del archivo, la establecemos
	this->fullscreen = fullscreen;
	this->windowSettings.antialiasingLevel = antialiasingLevel;
	if (this->fullscreen) {
		this->window = std::make_unique<sf::RenderWindow>(windowBounds, title, sf::Style::Fullscreen, windowSettings);
	}
	else {
		this->window = std::make_unique<sf::RenderWindow>(windowBounds, title, sf::Style::Close | sf::Style::Titlebar, windowSettings);
	}
	
	this->window->setFramerateLimit(framerateLimit);
	this->window->setVerticalSyncEnabled(verticalSyncEnabled);

}

void Game::initKeys() {
	// Obtiene las teclas soportadas supported_keys.ini
	std::ifstream ifs("config/supported_keys.ini");

	if (ifs.is_open()) {
		std::string key = "";
		int keyValue = 0;

		while (ifs >> key >> keyValue) {
			this->supportedKeys[key] = keyValue;
		}

	}
	ifs.close();
}

void Game::initStates() {
	// Establece el estado del menu al iniciar el juego
	this->states.push(std::make_unique<MainMenuState>(this->window.get(), &this->supportedKeys, &this->states));
}

// Constructor y destructor
Game::Game() {
	this->initVariables();
	this->initWindow();
	this->initKeys();
	this->initStates();
}

Game::~Game() {
	

	// Borra todos los estados en memoria
	while (!this->states.empty()) {
		// delete this->states.top();
		this->states.pop();
	}
}

void Game::updateDT() {
	// Calcula el dt, tiempo entre cada frame
	this->dt = this->dtClock.restart().asSeconds();

}

void Game::updateSFMLEvents() {
	while (this->window->pollEvent(this->sfEvent)) {
		if (this->sfEvent.type == sf::Event::Closed) {
			this->window->close();
		}
	}
}

void Game::update() {
	this->updateSFMLEvents();

	// Actualiza el estado actual
	if (!this->states.empty()) {
		this->states.top()->update(this->dt);

		if (this->states.top()->getQuit()) {
			// Si salimos del estado actual, borramos cosas
			this->states.top()->endState();
			// delete this->states.top(); 
			this->states.pop();
		}
	}
	else {
		// Si la pila de estados esta vacía, salimos de la aplicación
		this->endApplication();
		this->window->close();
	}

}

void Game::render() {
	this->window->clear();

	// Renderiza el estado actual
	if (!this->states.empty()) {
		this->states.top()->render();
	}

	this->window->display();
}

void Game::run() {
	while (this->window->isOpen()) {
		this->updateDT();
		this->update();
		this->render();
	}
}

void Game::endApplication() {
	std::cout << "Cerrando aplicación" << std::endl;
}
