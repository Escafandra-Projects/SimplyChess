#include "chess/Board.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

void Board::initVariables() {
	// Variables
	this->isMoving = false;
	this->endGame = false;
	this->promotionTurn = false;
	this->status = GameStatus::PLAYING;
	this->halfMoveClock = 0;
	this->positionHistory.clear();

	// Peon al paso
	for (int i = 0; i < 8; i++) {
		this->peonPaso[i][0] = false;
		this->peonPaso[i][1] = false;
	}

	// Casilla seleccionada
	this->selectedCell.setSize(sf::Vector2f(100.f, 100.f));
	this->selectedCell.setFillColor(sf::Color(222, 235, 127, 100));

	// Casilla de jaque
	this->jaqueCell.setSize(sf::Vector2f(100.f, 100.f));
	this->jaqueCell.setFillColor(sf::Color(255, 0, 0, 100));

	//Casillas blancas (+) y negras(-)
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2 == 0) board[i][j] = "+";
			else board[i][j] = "-";
		}
	}
	//Enroques
	this->castling[0] = true; // 0 - Enroque blancas corto
	this->castling[1] = true; // 1 - Enroque blancas largo
	this->castling[2] = true; // 2 - Enroque negras corto
	this->castling[3] = true; // 3 - Enroque negras largo
	this->castling[4] = true; // 4 - Movimiento del rey

	// Jaques al rey
	this->jaque[1] = false;
	this->jaque[0] = false;

	//Peones
	for (int i = 0; i < 8; i++) {
		board[6][i] = "PB";
		board[1][i] = "PN";
	}

	//Torres
	board[7][0] = "TB"; board[7][7] = "TB";
	board[0][0] = "TN"; board[0][7] = "TN";

	//Caballos
	board[7][1] = "CB"; board[7][6] = "CB";
	board[0][1] = "CN"; board[0][6] = "CN";

	//Alfiles
	board[7][2] = "AB"; board[7][5] = "AB";
	board[0][2] = "AN"; board[0][5] = "AN";

	//Reyes
	board[7][3] = "QB"; board[7][4] = "KB";
	board[0][3] = "QN"; board[0][4] = "KN";
}

void Board::initTextures(std::map<std::string, sf::Texture>& textures) {

	// Tablero
	if (!textures["BOARD"].loadFromFile("resources/images/Tablero.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD BACKGROUND");
	}
	// Torre negra
	if (!textures["TN"].loadFromFile("resources/images/pieces/TorreN.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD TORRE NEGRA");
	}
	// Torre blanca
	if (!textures["TB"].loadFromFile("resources/images/pieces/TorreB.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD TORRE BLANCA");
	}
	// Caballo negro
	if (!textures["CN"].loadFromFile("resources/images/pieces/CaballoN.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD CABALLO NEGRO");
	}
	// Caballo blanco
	if (!textures["CB"].loadFromFile("resources/images/pieces/CaballoB.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD CABALLO BLANCO");
	}


	// Alfil negro
	if (!textures["AN"].loadFromFile("resources/images/pieces/AlfilN.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD ALFIL NEGRO");
	}

	// Alfil blanco
	if (!textures["AB"].loadFromFile("resources/images/pieces/AlfilB.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD ALFIL BLANCO");
	}

	// Reina negra
	if (!textures["QN"].loadFromFile("resources/images/pieces/ReinaN.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD REINA NEGRA");
	}

	// Reina blanca
	if (!textures["QB"].loadFromFile("resources/images/pieces/ReinaB.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD REINA BLANCA");
	}

	// Rey negro
	if (!textures["KN"].loadFromFile("resources/images/pieces/ReyN.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD REY NEGRO");
	}

	// Rey blanco
	if (!textures["KB"].loadFromFile("resources/images/pieces/ReyB.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD REY BLANCO");
	}

	// Peon negro
	if (!textures["PN"].loadFromFile("resources/images/pieces/PeonN.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD PEON NEGRO");
	}

	// Peon blanco
	if (!textures["PB"].loadFromFile("resources/images/pieces/PeonB.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD PEON BLANCO");
	}

	// Coronación
	if (!textures["PROMOTION"].loadFromFile("resources/images/interface/promotionMenu.png")) {
		throw std::runtime_error("ERROR::GAME_STATE::FAILED TO LOAD PROMOTIOM");
	}

	// Menu de promocion
	this->promotionMenu = std::make_unique<PromotionMenu>(textures["PROMOTION"]);
}

void Board::initPieces(std::map<std::string, sf::Texture>& textures) {

	// Torre negra izq
	this->pieces[0][0] = std::make_unique<Piece>(0, 0, textures["TN"], PieceType::TORRE, 0);
	// Torre negra der
	this->pieces[1][0] = std::make_unique<Piece>(0, 7, textures["TN"], PieceType::TORRE, 0);
	// Torre blanca izq
	this->pieces[0][1] = std::make_unique<Piece>(7, 0, textures["TB"], PieceType::TORRE, 1);
	// Torre blanca der
	this->pieces[1][1] = std::make_unique<Piece>(7, 7, textures["TB"], PieceType::TORRE, 1);
	// Caballo negro izq
	this->pieces[2][0] = std::make_unique<Piece>(0, 1, textures["CN"], PieceType::CABALLO, 0);
	// Caballo negro der
	this->pieces[3][0] = std::make_unique<Piece>(0, 6, textures["CN"], PieceType::CABALLO, 0);
	// Caballo blanco izq
	this->pieces[2][1] = std::make_unique<Piece>(7, 1, textures["CB"], PieceType::CABALLO, 1);
	// Caballo blanco der
	this->pieces[3][1] = std::make_unique<Piece>(7, 6, textures["CB"], PieceType::CABALLO, 1);

	//alfil negro izq
	this->pieces[4][0] = std::make_unique<Piece>(0, 2, textures["AN"], PieceType::ALFIL, 0);

	//alfil negro der
	this->pieces[5][0] = std::make_unique<Piece>(0, 5, textures["AN"], PieceType::ALFIL, 0);

	//alfil blanco izq
	this->pieces[4][1] = std::make_unique<Piece>(7, 2, textures["AB"], PieceType::ALFIL, 1);

	//alfil blanco der
	this->pieces[5][1] = std::make_unique<Piece>(7, 5, textures["AB"], PieceType::ALFIL, 1);

	//reina negra
	this->pieces[6][0] = std::make_unique<Piece>(0, 3, textures["QN"], PieceType::REINA, 0);

	//reina blanca
	this->pieces[6][1] = std::make_unique<Piece>(7, 3, textures["QB"], PieceType::REINA, 1);

	//rey negro
	this->pieces[7][0] = std::make_unique<Piece>(0, 4, textures["KN"], PieceType::REY, 0);

	//rey blanco
	this->pieces[7][1] = std::make_unique<Piece>(7, 4, textures["KB"], PieceType::REY, 1);

	//peones
	for (int i=0; i<8; i++) {
		this->pieces[i+8][0] = std::make_unique<Piece>(1, i, textures["PN"], PieceType::PEON, 0);
		this->pieces[i+8][1] = std::make_unique<Piece>(6, i, textures["PB"], PieceType::PEON, 1);
	}
	this->background.setTexture(textures["BOARD"]);

}
void Board::startMove(sf::Vector2i mousePos, bool& turn) {
	sf::Vector2i pieceStartGridPos;
	pieceStartGridPos.y = (mousePos.x - BOARD_OFFSET_X) / CELL_SIZE;
	pieceStartGridPos.x = (mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE;



	// Obtenemos el objeto de la pieza a mover. Devuelve nullptr si no hay pieza
	this->movingPiece = this->getPiece(pieceStartGridPos.x, pieceStartGridPos.y);


	if (movingPiece) {
		this->selectedCell.setPosition(pieceStartGridPos.y * CELL_SIZE + BOARD_OFFSET_X, pieceStartGridPos.x * CELL_SIZE + BOARD_OFFSET_Y);
		// Permitimos selección solo si es el turno adecuado
		if (turn == movingPiece->getColor()) {
			this->isMoving = true;
		}
	}
}

void Board::endMove(sf::Vector2i mousePos, bool& turn, int& points1, int& points2) {

	sf::Vector2i pieceDesGridPos;
	pieceDesGridPos.y = (mousePos.x - BOARD_OFFSET_X) / CELL_SIZE;
	pieceDesGridPos.x = (mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE;
	sf::Vector2i pieceStartGridPos;
	pieceStartGridPos.y = movingPiece->getGridPosition().y;
	pieceStartGridPos.x = movingPiece->getGridPosition().x;

	this->menacedPiece = getPiece(pieceDesGridPos.x, pieceDesGridPos.y);

	// ========== FASE 1: Validar geometria en una COPIA del tablero ==========
	// checkMoveKing tiene efectos secundarios (mueve torre en el string del board para enroques)
	// Al usar checkMove(board), esos efectos secundarios solo afectan a testBoard, no a this->board
	BoardGrid testBoard = this->board;
	CastlingState testCastling = this->castling;

	if (!checkMove(turn, pieceStartGridPos, pieceDesGridPos, this->movingPiece, this->menacedPiece, testCastling, testBoard)) {
		this->isMoving = false;
		return;
	}

	// ========== FASE 2: Validar restricciones de jaque ==========
	bool isCastling = (this->movingPiece->getType() == PieceType::REY && std::abs(pieceStartGridPos.y - pieceDesGridPos.y) == 2);

	if (isCastling) {
		// Validar enroque usando tablero ORIGINAL (this->board NO ha sido modificado)
		if (!isCastlingLegal(turn, pieceStartGridPos, pieceDesGridPos, this->board)) {
			this->isMoving = false;
			return;
		}
	}

	// Simular el movimiento de la pieza en testBoard
	// (testBoard ya contiene los efectos secundarios de checkMoveKing para enroques: torre movida)
	testBoard[pieceDesGridPos.x][pieceDesGridPos.y] = testBoard[pieceStartGridPos.x][pieceStartGridPos.y];
	if ((pieceStartGridPos.x + pieceStartGridPos.y) % 2 != 0)
		testBoard[pieceStartGridPos.x][pieceStartGridPos.y] = "-";
	else
		testBoard[pieceStartGridPos.x][pieceStartGridPos.y] = "+";

	// Verificar que el rey no queda en jaque tras el movimiento
	if (isInCheck(turn, testBoard)) {
		this->isMoving = false;
		return;
	}

	// ========== FASE 3: Todo validado - aplicar al tablero real ==========
	this->board = testBoard;
	this->castling = testCastling;

	// Peon al paso
	if (this->peonPaso[pieceDesGridPos.y][!turn] && pieceStartGridPos.y != pieceDesGridPos.y && this->movingPiece->getType() == PieceType::PEON) {
		this->peonPasoMovement(turn, pieceStartGridPos, pieceDesGridPos);
	}
	for (int i = 0; i < 8; i++) {
		this->peonPaso[i][!turn] = false;
	}

	// Movemos visualmente
	this->movingPiece->move(pieceDesGridPos.x, pieceDesGridPos.y);

	// Coronación
	this->promotion(turn, pieceDesGridPos, false);

	// Movimiento de la torre en caso de enroques
	if (!castling[0]) {
		this->pieces[1][1]->move(7, 5);
		castling[0] = true;
	}
	if (!castling[1]) {
		this->pieces[0][1]->move(7, 3);
		castling[1] = true;
	}
	if (!castling[2]) {
		this->pieces[1][0]->move(0, 5);
		castling[2] = true;
	}
	if (!castling[3]) {
		this->pieces[0][0]->move(0, 3);
		castling[3] = true;
	}

	//Comemos pieza
	if (menacedPiece) {
		this->capturePiece(menacedPiece, turn, points1, points2);
	}

	// Regla de los 50 movimientos
	if (this->movingPiece->getType() == PieceType::PEON || menacedPiece != nullptr) {
		this->halfMoveClock = 0;
	} else {
		this->halfMoveClock++;
	}

	this->isMoving = false;

	// Cambiamos turno
	turn = !turn;

	// Triple repetición
	std::string currentHash = getPositionHash(turn);
	this->positionHistory[currentHash]++;

	// Comprobamos jaque al oponente
	this->jaque[0] = false;
	this->jaque[1] = false;

	if (isInCheck(turn, this->board)) {
		this->jaque[turn] = true;
		Piece* king = this->pieces[7][turn].get();
		this->jaqueCell.setPosition(king->getPosition().x, king->getPosition().y);

		if (isCheckmate(turn)) {
			this->status = GameStatus::CHECKMATE;
		}
	} else {
		if (isCheckmate(turn)) {
			this->status = GameStatus::STALEMATE;
		}
	}

	if (this->status == GameStatus::PLAYING) {
		if (this->halfMoveClock >= 100) {
			this->status = GameStatus::FIFTY_MOVE_RULE;
		} else if (this->positionHistory[currentHash] >= 3) {
			this->status = GameStatus::REPETITION;
		}
	}
}

void Board::capturePiece(Piece* menacedPiece, bool turn, int& points1, int& points2) {
	menacedPiece->setActive(false);
	menacedPiece->move(-100, -100);

	// Si capturamos al rey, la partida termina
	if (menacedPiece->getType() == PieceType::REY) {
		this->endGame = true;
	}

	if (turn) {
		points1 += menacedPiece->getPoints();
	}
	else {
		points2 += menacedPiece->getPoints();
	}
}

void Board::promotion(bool turn, sf::Vector2i& gridPos, bool isPromoting)
{

	if (isPromoting) {
		this->movingPiece->setColor(turn);
		if (turn) {
			if (this->promotionPiece == PieceType::REINA) {
				this->board[gridPos.x][gridPos.y] = "QB";
				this->movingPiece->setPieceType(PieceType::REINA);
				this->movingPiece->setTexture(this->promotionQB);
			}
			else if (this->promotionPiece == PieceType::CABALLO) {
				this->board[gridPos.x][gridPos.y] = "CB";
				this->movingPiece->setPieceType(PieceType::CABALLO);
				this->movingPiece->setTexture(this->promotionCB);
			}
			else if (this->promotionPiece == PieceType::ALFIL) {
				this->board[gridPos.x][gridPos.y] = "AB";
				this->movingPiece->setPieceType(PieceType::ALFIL);
				this->movingPiece->setTexture(this->promotionAB);
			}
			else if (this->promotionPiece == PieceType::TORRE) {
				this->board[gridPos.x][gridPos.y] = "TB";
				this->movingPiece->setPieceType(PieceType::TORRE);
				this->movingPiece->setTexture(this->promotionTB);
			}
		}
		else {
			if (this->promotionPiece == PieceType::REINA) {
				this->board[gridPos.x][gridPos.y] = "QN";
				this->movingPiece->setPieceType(PieceType::REINA);
				this->movingPiece->setTexture(this->promotionQN);
			}
			else if (this->promotionPiece == PieceType::CABALLO) {
				this->board[gridPos.x][gridPos.y] = "CN";
				this->movingPiece->setPieceType(PieceType::CABALLO);
				this->movingPiece->setTexture(this->promotionCN);
			}
			else if (this->promotionPiece == PieceType::ALFIL) {
				this->board[gridPos.x][gridPos.y] = "AN";
				this->movingPiece->setPieceType(PieceType::ALFIL);
				this->movingPiece->setTexture(this->promotionAN);
			}
			else if (this->promotionPiece == PieceType::TORRE) {
				this->board[gridPos.x][gridPos.y] = "TN";
				this->movingPiece->setPieceType(PieceType::TORRE);
				this->movingPiece->setTexture(this->promotionTN);
			}
		}
	}
	else {
		if (this->movingPiece->getType() == PieceType::PEON) {

				if (gridPos.x == 0) {
					this->promotionMenu->setPosition(gridPos.y * CELL_SIZE + BOARD_OFFSET_X, gridPos.x * CELL_SIZE + CELL_SIZE + BOARD_OFFSET_Y);
					this->promotionMenu->setShown(true, turn);
					this->promotionTurn = turn;
					this->promotionGridPos = gridPos;
				}
				if (gridPos.x == 7) {
					this->promotionMenu->setPosition(gridPos.y * CELL_SIZE + BOARD_OFFSET_X, gridPos.x * CELL_SIZE - 2 * CELL_SIZE + BOARD_OFFSET_Y);
					this->promotionMenu->setShown(true, turn);
					this->promotionTurn = turn;
					this->promotionGridPos = gridPos;
				}


		}
	}

}

void Board::peonPasoMovement(bool turn, sf::Vector2i startPos, sf::Vector2i desPos)
{
	//Quitamos el peon comido
	if (turn) {

		if (this->board[startPos.x][startPos.y] == "-") {
			board[3][desPos.y] = "+";
		}
		else {
			board[3][desPos.y] = "-";
		}
		this->peonPiece = this->getPiece(3, desPos.y);
	}
	else {
		if (this->board[startPos.x][startPos.y] == "-") {
			board[4][desPos.y] = "+";
		}
		else {
			board[4][desPos.y] = "-";
		}
		this->peonPiece = this->getPiece(4, desPos.y);
	}

	this->peonPiece->setActive(false);
	this->peonPiece->move(-100, -100);
}


Board::Board(std::map<std::string, sf::Texture>& textures) :
	promotionQB(textures["QB"]), promotionQN(textures["QN"]),
	promotionCB(textures["CB"]), promotionCN(textures["CN"]),
	promotionAB(textures["AB"]), promotionAN(textures["AN"]),
	promotionTB(textures["TB"]), promotionTN(textures["TN"]) {

	this->initTextures(textures);
	this->initVariables();
	this->initPieces(textures);
}

Board::~Board() {
}


bool Board::checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling) {


	if ((turn == movingPiece->getColor())&&(startPos!=desPos)) {
		if (menacedPiece) {
			if(menacedPiece->getColor() != movingPiece->getColor() && menacedPiece->isActive()) {
				if (movingPiece->checkMove(turn, startPos, desPos, this->board, castling, this->peonPaso)) {
					return true;
				}
				else {
					this->isMoving = false;
				}
			}
		}
		else {
			if (movingPiece->checkMove(turn, startPos, desPos, this->board, castling, this->peonPaso)) {
				return true;
			}
			else {
				this->isMoving = false;
			}
		}
	}

	return false;
}

bool Board::checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling, BoardGrid& checkBoard) {
	if ((turn == movingPiece->getColor())&&(startPos!=desPos)) {
		if (menacedPiece) {
			if(menacedPiece->getColor() != movingPiece->getColor() && menacedPiece->isActive()) {
				if (movingPiece->checkMove(turn, startPos, desPos, checkBoard, castling, this->peonPaso)) {
					return true;
				}
			}
		}
		else {
			if (movingPiece->checkMove(turn, startPos, desPos, checkBoard, castling, this->peonPaso)) {
				return true;
			}
		}
	}
	return false;
}


void Board::movePiece(bool& turn, int& points1, int& points2) {

	// Seleccionar pieza
	if (!this->isMoving) {


		// Comprobamos limites del tablero
		if (mousePos.x > BOARD_OFFSET_X && mousePos.x < BOARD_OFFSET_X + BOARD_SIZE && mousePos.y > BOARD_OFFSET_Y && mousePos.y < BOARD_OFFSET_Y + BOARD_SIZE) {
			this->startMove(mousePos, turn);
		}
	}
	// Seleccionar casilla de destino
	else {

		// Comprobamos limites del tablero
		if (mousePos.x > BOARD_OFFSET_X && mousePos.x < BOARD_OFFSET_X + BOARD_SIZE && mousePos.y > BOARD_OFFSET_Y && mousePos.y < BOARD_OFFSET_Y + BOARD_SIZE) {
			this->endMove(mousePos, turn, points1, points2);
		}
	}
}

Piece* Board::getPiece(int x, int y) {

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 2; j++) {
			if ((this->pieces[i][j]->getGridPosition().x == x && this->pieces[i][j]->getGridPosition().y == y))
			{
				return pieces[i][j].get();
			}
		}
	}
	return nullptr;
}

bool Board::getEndGame()
{
	return this->status != GameStatus::PLAYING;
}

GameStatus Board::getGameStatus() const
{
	return this->status;
}

void Board::forceEndGame(GameStatus status) {
	this->status = status;
	this->endGame = true;
}

bool Board::getIsMoving() const {
	return this->isMoving;
}

bool Board::isPromoting() const {
	return this->promotionMenu && this->promotionMenu->isShown();
}

std::string Board::getPositionHash(bool currentTurn) const {
	std::string hash;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			hash += this->board[i][j];
		}
	}
	hash += (currentTurn ? "W" : "B");
	for (bool c : this->castling) hash += (c ? "1" : "0");
	for (int i = 0; i < 8; ++i) {
		hash += (this->peonPaso[i][0] ? "1" : "0");
		hash += (this->peonPaso[i][1] ? "1" : "0");
	}
	return hash;
}

bool Board::isMenaced(bool turn, sf::Vector2i targetPos, Piece* targetPiece, BoardGrid& board, CastlingState& castling)
{
	int attackerColorIdx = turn ? 1 : 0;
	char expectedSuffix = turn ? 'B' : 'N';

	for (int i = 0; i < 16; i++) {
		Piece* attacker = this->pieces[i][attackerColorIdx].get();
		if (!attacker->isActive()) continue;

		sf::Vector2i attackerPos = attacker->getGridPosition();

		// Verificar que el atacante siga existiendo en esta posicion del tablero
		std::string cell = board[attackerPos.x][attackerPos.y];
		if (cell.length() < 2 || cell[1] != expectedSuffix) continue;

		if (this->checkMove(turn, attackerPos, targetPos, attacker, targetPiece, castling, board))
			return true;
	}
	return false;
}

bool Board::isInCheck(bool color, BoardGrid& board) {
	// Buscar la posicion del rey en el TABLERO (no en el objeto Piece)
	std::string kingCode = color ? "KB" : "KN";
	sf::Vector2i kingPos(-1, -1);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == kingCode) {
				kingPos = sf::Vector2i(i, j);
			}
		}
	}
	if (kingPos.x == -1) return false;

	Piece* king = this->pieces[7][color].get();
	return isMenaced(!color, kingPos, king, board, this->castling);
}

bool Board::isCheckmate(bool color) {
    for (int i = 0; i < 16; i++) {
        Piece* p = this->pieces[i][color].get();
        if (!p->isActive()) continue;

        sf::Vector2i startPos = p->getGridPosition();
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                sf::Vector2i desPos(x, y);
                if (startPos == desPos) continue;

                Piece* target = getPiece(x, y);

                // Saltar piezas amigas
                if (target && (target->getColor() == p->getColor() || !target->isActive())) continue;

                // Usar copias para evitar efectos secundarios de checkMoveKing
                BoardGrid testBoard = this->board;
                CastlingState testCastling = this->castling;

                if (p->checkMove(color, startPos, desPos, testBoard, testCastling, this->peonPaso)) {
                    // Simular el movimiento de la pieza sobre testBoard
                    testBoard[desPos.x][desPos.y] = testBoard[startPos.x][startPos.y];
                    if ((startPos.x + startPos.y) % 2 != 0) testBoard[startPos.x][startPos.y] = "-";
                    else testBoard[startPos.x][startPos.y] = "+";

                    if (!isInCheck(color, testBoard)) {
                        return false; // Hay al menos un movimiento legal
                    }
                }
            }
        }
    }
    return true; // No se encontró ningún movimiento legal
}

bool Board::isCastlingLegal(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& originalBoard) {
    // Si el rey esta en jaque, no puede enrocar
    if (isInCheck(turn, originalBoard)) return false;

    // Comprobar casilla intermedia
    sf::Vector2i intermediatePos = startPos;
    if (desPos.y > startPos.y) {
        intermediatePos.y += 1;
    } else {
        intermediatePos.y -= 1;
    }

    // Crear tablero con rey en la posicion intermedia (usando el tablero original)
    BoardGrid interBoard = originalBoard;
    std::string kingCode = turn ? "KB" : "KN";
    interBoard[intermediatePos.x][intermediatePos.y] = kingCode;
    if ((startPos.x + startPos.y) % 2 != 0) interBoard[startPos.x][startPos.y] = "-";
    else interBoard[startPos.x][startPos.y] = "+";

    if (isInCheck(turn, interBoard)) return false;

    return true;
}



void Board::update(sf::Vector2i mousePos, sf::RenderWindow& window) {
	// Actualiza las piezas
	for (int i = 0; i < 16; i++) {
		this->pieces[i][0]->update();
		this->pieces[i][1]->update();
	}




	// Esto esta MUY RARO. Para coronar utiliza posicion del raton calculada en State.cpp. Para mover el juego utiliza la posicion interna del Board
	if (!this->promotionMenu->isShown()) {

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) this->isMoving = false;
		this->mousePos = sf::Mouse::getPosition(window);
	}
	else {
		if (this->promotionMenu->isPressed(mousePos)) {
			this->promotionPiece = this->promotionMenu->getSelectedPiece(mousePos);
			this->promotionMenu->setShown(false, false);
			this->promotion(this->promotionTurn, this->promotionGridPos, true);
		}
	}
}

void Board::render(sf::RenderTarget& target)
{
	// Tablero
	target.draw(background);

	// Casilla selecionada
	if (this->isMoving) target.draw(this->selectedCell);

	// Jaques
	if (this->jaque[0] || this->jaque[1]) target.draw(this->jaqueCell);

	// Piezas
	for (int i = 0; i<16; i++) {
		this->pieces[i][0]->render(target);
		this->pieces[i][1]->render(target);
	}

	// Coronación
	this->promotionMenu->render(target);

}

void Board::showBoard() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == "+" || board[i][j] == "-") std::cout << board[i][j] << "  ";
			else std::cout << board[i][j] << " ";
		}
		std::cout << "\n";
	}
}
