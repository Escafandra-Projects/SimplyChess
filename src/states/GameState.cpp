#include "states/GameState.h"

#include <stdexcept>

// Inicialización
void GameState::initVariables() {
	this->paused = false;
	this->turn = true;

	this->player1 = "Player 1";
	this->player2 = "Player 2";
	this->points1 = 0;
	this->points2 = 0;
	this->gameOverReady = false;
	this->background.setPosition(sf::Vector2f(820.f, 0.f));

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

std::string GameState::buildScoreText() const {
	return "White: \n" + this->player1 + ". \nPoints: " + std::to_string(this->points1) +
		"\n\n\nBlack: \n" + this->player2 + ". \nPoints: " + std::to_string(this->points2);
}

void GameState::initText() {

	this->gameInfoText.setFont(this->font);
	this->gameInfoText.setCharacterSize(40);
	this->gameInfoText.setFillColor(sf::Color(75, 53, 47, 255));
	this->gameInfoText.setString(this->buildScoreText());
	this->gameInfoText.setPosition(850, 50);


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
}



void GameState::updateText() {
	//Actualizamos el texto del marcador
	this->gameInfoText.setString(this->buildScoreText());
}

// Funciones
void GameState::updateInput(float /*dt*/) {
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
}

void GameState::update(float dt) {

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
	// Si no se pasa target, se usa la ventana
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

}












