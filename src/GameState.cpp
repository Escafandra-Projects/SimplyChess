#include "GameState.h"

// Inicialización
void GameState::initVariables() {
	this->paused = false;
	this->turn = true;

	this->player1 = "Player 1";
	this->player2 = "Player 2";
	this->points1 = 0;
	this->points2 = 0;
	this->background.setPosition(sf::Vector2f(820.f, 0.f));

	// Mensaje fin del juego
	this->gameOverBox = std::make_unique<MessageBox>(font, "Game over", "Exit", this->textures["BUTTONS"] );
}   
   

void GameState::initFonts() {
	if (!this->font.loadFromFile("Resources/Fonts/Factory LJDS.ttf")) {
		throw("ERROR::MAINMENUSTATE::COULD NOT LOAD FONT");
	}
}

void GameState::initTextures() {
	// Botones
	if (!this->textures["BUTTONS"].loadFromFile("Resources/Images/Interface/buttons.png")) {
		throw "ERROR::MAIN_MENU_STATE::FAILED TO LOAD BUTTONS";
	}
	// Fondo   
	if (!this->textures["BACKGROUND"].loadFromFile("Resources/Images/Interface/background.png")) {
		throw "ERROR::MAIN_MENU_STATE::FAILED TO LOAD BACKGROUND";
	}
	this->background.setTexture(this->textures["BACKGROUND"]);
	this->background.scale(2.f, 2.f);
}

void GameState::initPauseMenu() {
	this->pauseMenu = std::make_unique<PauseMenu>(this->font);
	this->pauseMenu->addButton("EXIT", 50.f, 375.f, "Exit", this->textures["BUTTONS"]);
	this->pauseMenu->addButton("CONTINUE", 50.f, 250.f, "Continue", this->textures["BUTTONS"]);
}

void GameState::initBoard(std::map<std::string, sf::Texture>& textures) {
	this->board = std::make_unique<Board>(textures);
}

void GameState::initText() {

	this->gameInfoText.setFont(this->font);
	this->gameInfoText.setCharacterSize(40);
	this->gameInfoText.setFillColor(sf::Color(75, 53, 47, 255));
	string text = "White: \n" +
		this->player1 + ". \n"
		+ "Points: " + std::to_string(points1) + "\n\n\n"
		"Black: " + '\n'
		+ this->player2 + ". \n" +
		"Points: " + std::to_string(points2);

	this->gameInfoText.setString(text);
	this->gameInfoText.setPosition(850, 50);


}

void GameState::initKeybinds() {
	// Obtiene from gamestate_keybinds.ini
	std::ifstream ifs("Config/gamestate_keybinds.ini");

	if (ifs.is_open()) {
		std::string key = "";
		std::string key2 = "";

		while (ifs >> key >> key2) {
			this->keybinds[key] = this->supportedKeys->at(key2);
		}

	}
	ifs.close();
}

// Constructor & Destructor
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
}



void GameState::updateText() {
	//Actualizamos texto
	string text = "White: \n" +
		this->player1 + ". \n"
		+ "Points: " + std::to_string(points1) + "\n\n\n"
		"Black: " + '\n'
		+ this->player2 + ". \n" +
		"Points: " + std::to_string(points2);

	this->gameInfoText.setString(text);
}

// Funciones
void GameState::updateInput(const float& dt) {
	// Tecla de pausa
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("CLOSE"))) && this->getKeytime()) {
		if (!this->paused) {
			this->pauseState();
		}
		else {
			this->unpauseState();
		}
	}

	// Click izquierdo en la pieza para seleccionarla 
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->getKeytime()) {
		
		this->board->movePiece(this->turn, this->points1, this->points2);
		this->updateText();
	}

	// Boton del mensaje fin del juego
	if (this->gameOverBox->isButtonPressed()) {
		this->endState();
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
}

void GameState::update(const float& dt) {

	this->updateMousePositions();
	
	this->updateKeytime(dt);

	this->updateInput(dt);
	
	if (!this->paused) {
		if (this->board->getEndGame()) {
			if (this->turn) {
				this->gameOverBox->setText("  Game over.\n" + this->player2 + " wins");
			}
			else {
				this->gameOverBox->setText("  Game over.\n" + this->player1 + " wins");
			}
			this->gameOverBox->update(this->mousePosWindow);
		}
		else {
			this->board->update(this->mousePosWindow, *this->window);
		}
	}
	else {
		this->pauseMenu->update(this->mousePosWindow);
		this->updatePauseMenuButtons();
	}
	
}

void GameState::render(sf::RenderTarget* target) {
	// If target is default, use window as target
	if (!target) {
		target = this->window;
	}

	// Tablero y piezas
	this->board->render(*target);

	// Interfaz
	target->draw(this->background);
	target->draw(this->gameInfoText);

	if (this->board->getEndGame()) {
		this->gameOverBox->render(*target);
	}

	if (this->paused) {
		this->pauseMenu->render(*target);
	}

	if (this->board->getEndGame()) {

	}

}












