#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>

#include "chess/Piece.h"
#include "ui/PauseMenu.h"
#include "ui/PromotionMenu.h"

// Lado del tablero, en píxeles.
constexpr int BOARD_SIZE = 800;

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
	bool endGame;
	bool promotionTurn;

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
	/// Selecciona la pieza bajo el ratón si es del jugador en turno.
	void startMove(sf::Vector2i mousePos, bool& turn);
	/// Valida y aplica el movimiento a la casilla destino (jaque, enroque, etc.).
	void endMove(sf::Vector2i mousePos, bool& turn, int& points1, int& points2);
	/// Captura una pieza y suma sus puntos al jugador correspondiente.
	void capturePiece(Piece* menacedPiece, bool turn, int& points1, int& points2);
	/// Gestiona la coronación: muestra el menú o aplica la pieza elegida.
	void promotion(bool turn, sf::Vector2i& gridPos, bool isPromoting);
	/// Elimina el peón capturado en una captura al paso.
	void peonPasoMovement(bool turn, sf::Vector2i startPos, sf::Vector2i desPos);

public:
	// Constructor y destructor
	Board(std::map<std::string, sf::Texture>& textures);
	virtual ~Board();

	// Funciones
	/// Valida un movimiento sobre el tablero real.
	bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling);
	/// Valida un movimiento sobre un tablero de prueba (sin tocar el real).
	bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling, BoardGrid& checkBoard);
	/// Selecciona o mueve una pieza según el clic actual.
	void movePiece(bool& turn, int& points1, int& points2);
	/// Devuelve la pieza en la casilla (x, y) o nullptr si está vacía.
	Piece* getPiece(int x, int y);
	/// Indica si la partida ha terminado.
	bool getEndGame();

	/// Indica si la casilla objetivo está amenazada por el bando 'turn'.
	bool isMenaced(bool turn, sf::Vector2i targetPos, Piece* targetPiece, BoardGrid& board, CastlingState& castling);
	/// Indica si el rey del color dado está en jaque en el tablero dado.
	bool isInCheck(bool color, BoardGrid& board);
	/// Indica si el color dado está en jaque mate (sin movimientos legales).
	bool isCheckmate(bool color);
	/// Comprueba que el enroque es legal (rey no en jaque ni pasa por casilla amenazada).
	bool isCastlingLegal(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& originalBoard);

	// Funciones principales
	/// Actualiza el tablero y el menú de coronación.
	void update(sf::Vector2i mousePos, sf::RenderWindow& window);
	/// Dibuja el tablero, las piezas y los resaltados.
	void render(sf::RenderTarget& target);

	// DEPURACIÓN
	/// Imprime el tablero por consola (utilidad de depuración).
	void showBoard();

};
