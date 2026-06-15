#pragma once

#include "State.h"
#include "Piece.h"
#include "Board.h"
#include <memory>
#include "MessageBox.h"
class GameState : public State {
private:
	// Variables
	/* Interfaz */
	sf::Font font;
	std::unique_ptr<PauseMenu> pauseMenu;
	std::unique_ptr<MessageBox> gameOverBox;
	sf::Text gameInfoText;
	sf::Sprite background;
	/* Tablero */
	std::unique_ptr<Board> board;
	/* Info de la partida */
	std::string player1, player2;
	int points1, points2;
	bool turn;

	// Inicialización
	void initVariables();
	void initKeybinds();
	void initFonts();
	void initTextures();
	void initPauseMenu();
	void initBoard(std::map<std::string, sf::Texture>& textures);
	void initText();

public:
	// Constructor & Destructor
	GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~GameState();

	// Funciones
	/* Actualizar */
	void updateText();
	void updateInput(const float& dt);
	void updatePauseMenuButtons();
	void update(const float& dt);
	/* Renderizar */
	void render(sf::RenderTarget* target = nullptr);
};