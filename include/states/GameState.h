#pragma once

#include "states/State.h"
#include "chess/Piece.h"
#include "chess/Board.h"
#include <memory>
#include "ui/MessageBox.h"

/// Estado de partida: contiene el tablero, el marcador, el menú de pausa
/// y el mensaje de fin de juego.
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
	bool previousTurn;
	
	/* Reloj */
	float baseTime;
	float increment;
	float timeWhite;
	float timeBlack;
	// El reloj no corre hasta que las blancas completan su primer movimiento.
	bool clockStarted;

	// Evita que el clic que da el mate cierre el cuadro de fin de partida:
	// solo se acepta el botón Exit tras soltar el ratón al menos una vez.
	bool gameOverReady;

	// Inicialización
	/// Inicializa las variables y el mensaje de fin de partida.
	void initVariables();
	/// Carga la asignación de teclas desde config/gamestate_keybinds.ini.
	void initKeybinds();
	/// Carga la fuente de texto.
	void initFonts();
	/// Carga las texturas de interfaz (botones, fondo).
	void initTextures();
	/// Crea el menú de pausa y sus botones.
	void initPauseMenu();
	/// Crea el tablero con sus texturas.
	void initBoard(std::map<std::string, sf::Texture>& textures);
	/// Prepara el texto informativo de la partida.
	void initText();

public:
	// Constructor y destructor
	GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~GameState();

	// Funciones
	/// Construye el texto del marcador (jugadores y puntos).
	std::string buildScoreText() const;
	/* Actualizar */
	/// Refresca el texto del marcador (jugadores y puntos).
	void updateText();
	/// Procesa la entrada: pausa, clics sobre el tablero y fin de juego.
	void updateInput(float dt) override;
	/// Gestiona los botones del menú de pausa (salir / continuar).
	void updatePauseMenuButtons();
	/// Actualiza el estado de la partida o el menú de pausa.
	void update(float dt) override;
	/* Renderizar */
	/// Dibuja el tablero, la interfaz y, si procede, los menús.
	void render(sf::RenderTarget* target = nullptr) override;
};
