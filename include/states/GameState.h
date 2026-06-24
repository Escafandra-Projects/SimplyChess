#pragma once

#include "states/State.h"
#include "states/GameConfig.h"
#include "chess/Piece.h"
#include "chess/Board.h"
#include <memory>
#include <array>
#include "ui/MessageBox.h"
#include "ui/MoveListPanel.h"
#include "ui/MenuButton.h"
#include "ui/WoodPanel.h"
#include "chess/GameSnapshot.h"
#include "chess/AIEngine.h"
#include <thread>
#include <atomic>
#include <future>

/// Estado de partida: contiene el tablero, el marcador, el menú de pausa
/// y el mensaje de fin de juego.
class GameState : public State {
private:
	// Variables
	/* Interfaz */
	sf::Font font;
	sf::Font panelFont;     // Gameplay.ttf para panel lateral
	std::unique_ptr<PauseMenu> pauseMenu;
	std::unique_ptr<MessageBox> gameOverBox;
	std::unique_ptr<MessageBox> confirmResignBox;
	bool confirmResignActive;
	std::unique_ptr<MessageBox> drawOfferBox;
	bool drawOfferActive;
	std::unique_ptr<MessageBox> drawRejectedBox;
	bool drawRejectedActive;
	std::unique_ptr<MoveListPanel> moveListPanel;
	sf::Text gameInfoText;
	sf::Sprite background;

	// ── Nuevo panel lateral (dark-wood theme) ──────────────────
	sf::RectangleShape bgRect;
	sf::VertexArray vignetteVA;
	WoodPanel sidePanel;

	// Labels tablero
	sf::Text labelNegras, labelBlancas;
	sf::RectangleShape dotNegras, dotBlancas;

	// Fila jugador negro (top)
	sf::RectangleShape blackRowBg;
	sf::RectangleShape blackKingBox;
	sf::Sprite blackKingSprite;
	sf::Text blackNameTxt, blackStatusTxt, blackTimerTxt;

	// Fila jugador blanco (bottom, activo)
	sf::RectangleShape whiteRowBg, turnBar;
	sf::RectangleShape whiteKingBox;
	sf::Sprite whiteKingSprite;
	sf::Text whiteNameTxt, whiteStatusTxt, whiteTimerTxt;

	// Separadores y ventaja
	std::array<sf::RectangleShape, 5> sepLines;
	sf::Text blackAdvTxt, whiteAdvTxt;

	// Botones de acción
	std::vector<MenuButton> actionBtns;
	bool mouseHeldForActionBtns;

	sf::Clock glowClock;
	// ───────────────────────────────────────────────────────────
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

	// Estado del botón izquierdo el frame anterior, para detectar los flancos de
	// pulsación y de soltar (necesario para distinguir clic de arrastre).
	bool mouseHeldLastFrame;

	// Undo/Redo
	std::vector<GameSnapshot> undoStack;
	int currentMoveIndex;
	void captureStateForUndo();
	void undo();
	void redo();

	// AI
	GameConfig config;
	bool flipped;       // true = tablero volteado (jugador con negras frente a la IA)
	bool aiPlaysWhite;  // color que juega la IA en modo IA
	bool aiMode;
	int aiDifficulty; // Profundidad máxima
	bool aiIsThinking;
	std::atomic<bool> aiStopFlag;
	std::future<FastMove> aiFutureMove;
	void startAIThinking();
	void processAIMove();

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
	/// Construye el panel lateral con tema oscuro de madera.
	void initGamePanel();
	/// Actualiza timers, indicador de turno y botones del panel lateral.
	void updateGamePanel();

public:
	// Constructor y destructor
	GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states, const GameConfig& config);
	// Constructor de conveniencia para el menú: arma una GameConfig básica según el modo.
	GameState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states, bool forceAiMode);
	virtual ~GameState();

	// Funciones
	/// Construye el texto del marcador (jugadores y puntos).
	std::string buildScoreText() const;
	/* Actualizar */
	/// Refresca el texto del marcador (jugadores y puntos).
	void updateText();
	/// Procesa eventos discretos (teclado, etc.).
	void handleEvent(const sf::Event& event) override;
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
