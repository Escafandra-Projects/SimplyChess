#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "chess/Piece.h"
#include "ui/PauseMenu.h"
#include "ui/PromotionMenu.h"
#include "chess/MoveHistory.h"
#include "chess/GameSnapshot.h"

// Lado del tablero, en píxeles.
constexpr int BOARD_SIZE = 800;

enum class GameStatus {
	PLAYING,
	CHECKMATE,
	STALEMATE,
	FIFTY_MOVE_RULE,
	REPETITION,
	TIMEOUT
};

/// El tablero: mantiene la representación de la posición, las piezas y las
/// reglas (movimientos, enroque, captura al paso, coronación, jaque y mate).
class Board {
private:
	// Variables
	sf::Sprite background;
	BoardGrid board;
	CastlingState castling;
	std::array<bool, 2> jaque;
	EnPassantState peonPaso;


	/*Texturas de piezas de promocion*/
	sf::Texture& promotionQB;
	sf::Texture& promotionQN;
	sf::Texture& promotionCB;
	sf::Texture& promotionCN;
	sf::Texture& promotionAB;
	sf::Texture& promotionAN;
	sf::Texture& promotionTB;
	sf::Texture& promotionTN;

	/* Interfaz */
	sf::RectangleShape selectedCell;
	sf::RectangleShape jaqueCell;
	sf::RectangleShape lastMoveStartCell;
	sf::RectangleShape lastMoveEndCell;
	bool hasLastMove;
	std::vector<sf::CircleShape> legalMovesShapes;
	std::unique_ptr<PromotionMenu> promotionMenu;

	/* Piezas */
	Piece* movingPiece;
	Piece* menacedPiece;
	Piece* peonPiece; // Pieza eliminada de peon al paso
	PieceType promotionPiece;

	/*
		Color 0 negra 1 blanca
		Torres 0, 1
		Caballos 2, 3
		Alfiles 4 5
		Reina 6
		Rey 7
	*/
	std::array<std::array<std::unique_ptr<Piece>, 2>, 16> pieces;
	bool isMoving;
	bool isDragging;
	bool endGame;
	bool promotionTurn;
	bool isAIMove;

	GameStatus status;
	int halfMoveClock;
	std::map<std::string, int> positionHistory;
	MoveHistory moveHistory;

	sf::Vector2i mousePos;
	sf::Vector2i promotionGridPos;


	// Funciones de inicialización
	/// Inicializa la posición inicial del tablero y sus flags.
	void initVariables();
	/// Carga las texturas del tablero y las piezas.
	void initTextures(std::map<std::string, sf::Texture>& textures);
	/// Crea los objetos Piece en su posición inicial.
	void initPieces(std::map<std::string, sf::Texture>& textures);

	// Funciones privadas
	/// Genera un string que representa el estado actual para detectar repeticiones.
	std::string getPositionHash(bool currentTurn) const;

	/// Calcula los movimientos legales de la pieza en startPos y rellena legalMovesShapes
	void calculateLegalMoves(bool turn, sf::Vector2i startPos);

	/// Selecciona la pieza bajo el ratón si es del jugador en turno.
	void startMove(sf::Vector2i mousePos, bool& turn);
	/// Valida y aplica el movimiento a la casilla destino (jaque, enroque, etc.).
	void endMove(sf::Vector2i mousePos, bool& turn, int& points1, int& points2);
	/// Convierte una posición de ratón (píxeles) a casilla de rejilla (x=fila, y=columna).
	sf::Vector2i mouseToGrid(sf::Vector2i mousePos) const;
	/// Indica si la posición de ratón cae dentro del área jugable del tablero.
	bool isInsideBoard(sf::Vector2i mousePos) const;
	/// Captura una pieza y suma sus puntos al jugador correspondiente.
	void capturePiece(Piece* menacedPiece, bool turn, int& points1, int& points2);
	/// Gestiona la coronación: muestra el menú o aplica la pieza elegida.
	void promotion(bool turn, sf::Vector2i& gridPos, bool isPromoting);
	/// Elimina el peón capturado en una captura al paso.
	void peonPasoMovement(bool turn, sf::Vector2i startPos, sf::Vector2i desPos);

	bool needsDisambiguation(bool turn, PieceType type, sf::Vector2i from, sf::Vector2i to) const;
	bool isMoveLegal(bool turn, sf::Vector2i from, sf::Vector2i to) const;

public:
	// Constructor y destructor
	Board(std::map<std::string, sf::Texture>& textures);
	virtual ~Board();

	// Funciones
	/// Valida un movimiento sobre el tablero real.
	bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling);
	/// Valida un movimiento sobre un tablero de prueba (sin tocar el real).
	bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling, BoardGrid& checkBoard, EnPassantState& peonPaso);
	/// Selecciona o mueve una pieza según el clic actual.
	void movePiece(bool& turn, int& points1, int& points2);
	/// Flanco de pulsación: selecciona una pieza, la re-agarra o completa un movimiento (modo dos clics).
	void onPress(sf::Vector2i mousePos, bool& turn, int& points1, int& points2);
	/// Ejecuta un movimiento del bot
	bool applyAIMove(int fromX, int fromY, int toX, int toY, PieceType promotion, bool& turn, int& points1, int& points2);
	/// Mientras se mantiene pulsado: la pieza agarrada sigue al cursor.
	void onDrag(sf::Vector2i mousePos);
	/// Flanco de soltar: suelta la pieza en la casilla destino o cancela el arrastre.
	void onRelease(sf::Vector2i mousePos, bool& turn, int& points1, int& points2);
	/// Devuelve la pieza en la casilla (x, y) o nullptr si está vacía.
	Piece* getPiece(int x, int y);
	/// Indica si la partida ha terminado.
	bool getEndGame();
	/// Devuelve el estado actual de la partida.
	GameStatus getGameStatus() const;
	/// Devuelve si hay una pieza seleccionada moviéndose.
	bool getIsMoving() const;
	/// Devuelve si el menú de promoción está abierto.
	bool isPromoting() const;
	/// Fuerza el final de la partida por un estado específico (ej. TIMEOUT)
	void forceEndGame(GameStatus status);

	/// Indica si la casilla objetivo está amenazada por el bando 'turn'.
	bool isMenaced(bool turn, sf::Vector2i targetPos, Piece* targetPiece, BoardGrid& board, CastlingState& castling);
	/// Indica si el rey del color dado está en jaque en el tablero dado.
	bool isInCheck(bool color, BoardGrid& board);
	/// Indica si el color dado está en jaque mate (sin movimientos legales).
	bool isCheckmate(bool color);
	/// Comprueba que el enroque es legal (rey no en jaque ni pasa por casilla amenazada).
	bool isCastlingLegal(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& originalBoard);
	/// Indica si el estado actual es GameStatus::PLAYING.
	bool isPlaying() const;
	/// Indica si alguna pieza se está animando.
	bool isAnyPieceAnimating() const;

	/* Movimiento y Selección */
	/// Devuelve true si la partida no ha terminado y hay una pieza siendo movida.
	bool IsMoving() const;

	/// Gestiona la selección de una pieza o el destino de un movimiento.
	void updateInput(const sf::Vector2i& mousePosWindow, bool turn);
	/// Actualiza el tablero y el menú de coronación.
	void update(sf::Vector2i mousePos, sf::RenderWindow& window);
	/// Actualiza las animaciones de las piezas.
	void updateAnimations(float dt);
	/// Dibuja el tablero, casillas resaltadas, piezas y el menú de coronación.
	void render(sf::RenderTarget& target);

	const MoveHistory& getMoveHistory() const { return moveHistory; }

	std::vector<Piece*> getCapturedPieces(bool color) const;

	/// Captura un snapshot del estado actual del tablero
	GameSnapshot captureSnapshot() const;

	/// Restaura el estado del tablero desde un snapshot
	void restoreSnapshot(const GameSnapshot& snapshot, std::map<std::string, sf::Texture>& textures);

	/// Obtiene la textura correcta para un tipo y color de pieza
	static std::string getTextureKey(PieceType type, bool color);

	// DEPURACIÓN
	/// Imprime el tablero por consola (utilidad de depuración).
	void showBoard();

};
