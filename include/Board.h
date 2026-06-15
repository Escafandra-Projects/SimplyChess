 #pragma once
#include <map>
#include <string>

#define BOARD_SIZE  800 

#include "Piece.h"
#include <memory>
#include "PauseMenu.h"
#include "PromotionMenu.h"
  
class Board { 
private:  
	// Variables  
	sf::Sprite background;
    std::string board[8][8];
	std::string tempBoard[8][8];
	bool castling[5];
	bool jaque[2];
	bool peonPaso[8][2];


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
	unsigned short int promotionPiece;

	/*
		Color 0 negra 1 blanca
		Torres 0, 1
		Caballos 2, 3
		Alfiles 4 5   
		Reina 6
		Rey 7
	*/
	std::unique_ptr<Piece> pieces[16][2];
	bool isMoving;
	bool endGame;
	bool promotionTurn;

	sf::Vector2i mousePos;
	sf::Vector2i promotionGridPos;
	

	// Init functions
	void initVariables();
	void initTextures(std::map<std::string, sf::Texture>& textures);
	void initPieces(std::map<std::string, sf::Texture>& textures);

	// Funciones privadas
	void startMove(sf::Vector2i mousePos, bool& turn);
	void endMove(sf::Vector2i mousePos, bool& turn, int& points1, int& points2);
	void capturePiece(Piece* menacedPiece, bool turn, int& points1, int& points2);
	void promotion(bool turn, sf::Vector2i& gridPos, bool isPromoting);
	void peonPasoMovement(bool turn, sf::Vector2i startPos, sf::Vector2i desPos);
	void copyBoard(string board[8][8], string newBoard[8][8]);
	
public:
	// Constructor & Destructor
	Board(std::map<std::string, sf::Texture>& textures);
	virtual ~Board();

	// Functions
	bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, bool castling[5]);
	bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, bool castling[5], std::string checkBoard[8][8]);
	void movePiece(bool& turn, int& points1, int& points2);
	Piece* getPiece(int x, int y);
	bool getEndGame();

	bool isMenaced(bool turn, sf::Vector2i targetPos, Piece* targetPiece, std::string board[8][8], bool castling[5]);
	bool isInCheck(bool color, std::string board[8][8]);
	bool isCheckmate(bool color);
	bool simulateAndValidate(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece);
	bool isCastlingLegal(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, std::string originalBoard[8][8]);

	// Core functions
	void update(sf::Vector2i mousePos, sf::RenderWindow& window);
	void render(sf::RenderTarget& target);
	  
	// DEBUG
	void showBoard();

};
