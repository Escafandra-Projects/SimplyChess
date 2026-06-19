#include "states/GameState.h"
#include <iomanip>
#include <sstream>

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
	this->gameOverReady = false;
	this->mouseHeldLastFrame = false;
	this->background.setPosition(sf::Vector2f(820.f, 0.f));

	std::ifstream ifs("config/game.ini");
	if (ifs.is_open()) {
		ifs >> this->baseTime >> this->increment;
		ifs.close();
	} else {
		this->baseTime = 300.0f;
		this->increment = 0.0f;
	}
	this->timeWhite = this->baseTime;
	this->timeBlack = this->baseTime;
	this->clockStarted = false;

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
		return "White: \n" + this->player1 + ". \nPoints: " + std::to_string(this->points1) +
			"\n\n\nBlack: \n" + this->player2 + ". \nPoints: " + std::to_string(this->points2);
	}
	return "White: \n" + this->player1 + ". \nPoints: " + std::to_string(this->points1) +
		"\nTime: " + formatTime(this->timeWhite) + 
		"\n\n\nBlack: \n" + this->player2 + ". \nPoints: " + std::to_string(this->points2) +
		"\nTime: " + formatTime(this->timeBlack);
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
}

	// Movimiento: admite dos clics (clic en pieza, clic en destino) y arrastrar-soltar.
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	if (!this->paused && !this->board->getEndGame() && !this->board->isPromoting()) {
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
}

void GameState::update(float dt) {

	this->updateMousePositions();
	
	this->updateKeytime(dt);

	this->updateInput(dt);
	
	if (!this->paused) {
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
			}
			this->updateText();
		}

		if (this->board->getEndGame()) {
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
			this->gameOverBox->update(this->mousePosWindow);
		}
		else {
			this->board->updateAnimations(dt);
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












