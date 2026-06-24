#include "chess/Board.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "core/AudioSystem.h"

void Board::initVariables() {
	// Variables
	this->isMoving = false;
	this->isDragging = false;
	this->endGame = false;
	this->promotionTurn = false;
	this->isAIMove = false;
	this->promotionPiece = PieceType::REINA;
	this->status = GameStatus::PLAYING;
	this->halfMoveClock = 0;
	this->positionHistory.clear();

	// Peon al paso
	for (int i = 0; i < 8; i++) {
		this->peonPaso[i][0] = false;
		this->peonPaso[i][1] = false;
	}

	// Ultimo movimiento
	this->hasLastMove = false;
	this->lastMoveStartCell.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
	this->lastMoveStartCell.setFillColor(sf::Color(222, 235, 127, 100));
	this->lastMoveEndCell.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
	this->lastMoveEndCell.setFillColor(sf::Color(222, 235, 127, 100));
	this->legalMovesShapes.clear();

	// Casilla seleccionada
	this->selectedCell.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
	this->selectedCell.setFillColor(sf::Color(222, 235, 127, 100));

	// Casilla de jaque
	this->jaqueCell.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
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
	textures["BOARD"].setSmooth(true);
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
	this->background.setPosition(21.f, 20.f);
	this->background.setScale(0.95f, 0.95f);

}

void Board::calculateLegalMoves(bool turn, sf::Vector2i startPos) {
	this->legalMovesShapes.clear();
	
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			sf::Vector2i desPos(x, y);
			if (startPos == desPos) continue;

			Piece* target = getPiece(x, y);

			// Saltar piezas amigas
			if (target && (target->getColor() == turn || !target->isActive())) continue;

			// Usar copias para evitar efectos secundarios
			BoardGrid testBoard = this->board;
			CastlingState testCastling = this->castling;
			EnPassantState testPeonPaso = this->peonPaso;

			if (this->movingPiece->checkMove(turn, startPos, desPos, testBoard, testCastling, testPeonPaso)) {
				// Validar enroque específicamente
				bool isCastling = (this->movingPiece->getType() == PieceType::REY && std::abs(startPos.y - desPos.y) == 2);
				if (isCastling) {
					if (!isCastlingLegal(turn, startPos, desPos, this->board)) continue;
				}

				// Simular movimiento
				testBoard[desPos.x][desPos.y] = testBoard[startPos.x][startPos.y];
				if ((startPos.x + startPos.y) % 2 != 0) testBoard[startPos.x][startPos.y] = "-";
				else testBoard[startPos.x][startPos.y] = "+";

				if (!isInCheck(turn, testBoard)) {
					// Movimiento legal
					bool isCapture = (target && target->isActive());
					// Detección de captura al paso: es peón, se mueve en diagonal y no hay objetivo en la casilla final
					if (!isCapture && this->movingPiece->getType() == PieceType::PEON && startPos.y != desPos.y) {
						isCapture = true;
					}

					if (isCapture) {
						// Captura: círculo hueco
						float radius = CELL_SIZE * 0.45f;
						sf::CircleShape circle(radius);
						circle.setFillColor(sf::Color::Transparent);
						circle.setOutlineColor(sf::Color(0, 0, 0, 45));
            circle.setOutlineThickness(CELL_SIZE * 0.05f);               // de B
            circle.setPosition(colToPixelX(desPos.y) + (CELL_SIZE - 2.f * radius) / 2.f,   // orientación A + centrado B
                                rowToPixelY(desPos.x) + (CELL_SIZE - 2.f * radius) / 2.f);
						this->legalMovesShapes.push_back(circle);
					} else {
						// Movimiento normal: punto
						float radius = CELL_SIZE * 0.15f;
						sf::CircleShape dot(radius);
						dot.setFillColor(sf::Color(0, 0, 0, 45));
            dot.setPosition(colToPixelX(desPos.y) + (CELL_SIZE - 2.f * radius) / 2.f,
                rowToPixelY(desPos.x) + (CELL_SIZE - 2.f * radius) / 2.f);
						this->legalMovesShapes.push_back(dot);
					}
				}
			}
		}
	}
}

void Board::startMove(sf::Vector2i mousePos, bool& turn) {
	sf::Vector2i pieceStartGridPos;
	pieceStartGridPos.y = pixelXToCol(mousePos.x);
	pieceStartGridPos.x = pixelYToRow(mousePos.y);



	// Obtenemos el objeto de la pieza a mover. Devuelve nullptr si no hay pieza
	this->movingPiece = this->getPiece(pieceStartGridPos.x, pieceStartGridPos.y);


	if (movingPiece) {
		this->selectedCell.setPosition(colToPixelX(pieceStartGridPos.y), rowToPixelY(pieceStartGridPos.x));
		// Permitimos selección solo si es el turno adecuado
		if (turn == movingPiece->getColor()) {
			this->isMoving = true;
			this->calculateLegalMoves(turn, pieceStartGridPos);
		}
	}
}

void Board::endMove(sf::Vector2i mousePos, bool& turn, int& points1, int& points2) {
	if (!movingPiece) return;

	sf::Vector2i pieceDesGridPos;
	pieceDesGridPos.y = pixelXToCol(mousePos.x);
	pieceDesGridPos.x = pixelYToRow(mousePos.y);
	sf::Vector2i pieceStartGridPos;
	pieceStartGridPos.y = movingPiece->getGridPosition().y;
	pieceStartGridPos.x = movingPiece->getGridPosition().x;

	this->menacedPiece = getPiece(pieceDesGridPos.x, pieceDesGridPos.y);

	// Construct and populate MoveRecord before the board is modified
	MoveRecord record;
	record.pieceType = movingPiece->getType();
	record.color = turn;
	record.from = pieceStartGridPos;
	record.to = pieceDesGridPos;
	
	// Determine special flags
	record.isEnPassant = (movingPiece->getType() == PieceType::PEON && pieceStartGridPos.y != pieceDesGridPos.y && peonPaso[pieceDesGridPos.y][!turn] && menacedPiece == nullptr);
	record.isCapture = (menacedPiece != nullptr) || record.isEnPassant;
	record.isCastleShort = (movingPiece->getType() == PieceType::REY && (pieceDesGridPos.y - pieceStartGridPos.y == 2));
	record.isCastleLong = (movingPiece->getType() == PieceType::REY && (pieceDesGridPos.y - pieceStartGridPos.y == -2));
	record.isPromotion = (movingPiece->getType() == PieceType::PEON && (pieceDesGridPos.x == 0 || pieceDesGridPos.x == 7));
	record.promotedTo = PieceType::PEON; // placeholder
	record.isCheck = false; // will be updated at the end of endMove()
	record.isCheckmate = false; // will be updated at the end of endMove()

	// captured piece info
	Piece* capturedPiece = menacedPiece;
	if (!capturedPiece && record.isEnPassant) {
		int capRow = turn ? 3 : 4;
		capturedPiece = const_cast<Board*>(this)->getPiece(capRow, pieceDesGridPos.y);
	}
	record.capturedPieceIndex = -1;
	record.capturedType = PieceType::PEON; // default
	if (capturedPiece) {
		record.capturedType = capturedPiece->getType();
		for (int i = 0; i < 16; ++i) {
			if (pieces[i][!turn].get() == capturedPiece) {
				record.capturedPieceIndex = i;
				break;
			}
		}
	}

	// Disambiguation
	bool fileConflict = false;
	bool rankConflict = false;
	bool otherCanMove = false;
	if (record.pieceType != PieceType::PEON && record.pieceType != PieceType::REY) {
		for (int i = 0; i < 16; ++i) {
			Piece* p2 = pieces[i][turn].get();
			if (p2 && p2->isActive() && p2->getType() == record.pieceType) {
				sf::Vector2i p2Pos = p2->getGridPosition();
				if (p2Pos != pieceStartGridPos) {
					if (isMoveLegal(turn, p2Pos, pieceDesGridPos)) {
						otherCanMove = true;
						if (p2Pos.y == pieceStartGridPos.y) {
							fileConflict = true;
						}
						if (p2Pos.x == pieceStartGridPos.x) {
							rankConflict = true;
						}
					}
				}
			}
		}
	}
	record.needsFileDisambiguation = false;
	record.needsRankDisambiguation = false;
	if (otherCanMove) {
		if (fileConflict && !rankConflict) {
			record.needsRankDisambiguation = true;
		} else if (!fileConflict && rankConflict) {
			record.needsFileDisambiguation = true;
		} else if (fileConflict && rankConflict) {
			record.needsFileDisambiguation = true;
			record.needsRankDisambiguation = true;
		} else {
			record.needsFileDisambiguation = true;
		}
	}

	// ========== FASE 1: Validar geometria en una COPIA del tablero ==========
	// checkMoveKing tiene efectos secundarios (mueve torre en el string del board para enroques)
	// Al usar checkMove(board), esos efectos secundarios solo afectan a testBoard, no a this->board
	BoardGrid testBoard = this->board;
	CastlingState testCastling = this->castling;
	EnPassantState testPeonPaso = this->peonPaso;

	if (!checkMove(turn, pieceStartGridPos, pieceDesGridPos, this->movingPiece, this->menacedPiece, testCastling, testBoard, testPeonPaso)) {
		this->isMoving = false;
		this->legalMovesShapes.clear();
		return;
	}

	// ========== FASE 2: Validar restricciones de jaque ==========
	bool isCastling = (this->movingPiece->getType() == PieceType::REY && std::abs(pieceStartGridPos.y - pieceDesGridPos.y) == 2);

	if (isCastling) {
		// Validar enroque usando tablero ORIGINAL (this->board NO ha sido modificado)
		if (!isCastlingLegal(turn, pieceStartGridPos, pieceDesGridPos, this->board)) {
			this->isMoving = false;
			this->legalMovesShapes.clear();
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
		this->legalMovesShapes.clear();
		return;
	}

	// ========== FASE 3: Todo validado - aplicar al tablero real ==========
	this->board = testBoard;
	this->castling = testCastling;
	this->peonPaso = testPeonPaso;

	// Actualizar celdas del último movimiento
	this->lastMoveStartCell.setPosition(colToPixelX(pieceStartGridPos.y), rowToPixelY(pieceStartGridPos.x));
	this->lastMoveEndCell.setPosition(colToPixelX(pieceDesGridPos.y), rowToPixelY(pieceDesGridPos.x));
	this->hasLastMove = true;
	this->legalMovesShapes.clear();

	// Peon al paso
	if (this->peonPaso[pieceDesGridPos.y][!turn] && pieceStartGridPos.y != pieceDesGridPos.y && this->movingPiece->getType() == PieceType::PEON && menacedPiece == nullptr) {
		this->peonPasoMovement(turn, pieceStartGridPos, pieceDesGridPos);
	}
	for (int i = 0; i < 8; i++) {
		this->peonPaso[i][!turn] = false;
	}

	// Movemos visualmente
	this->movingPiece->move(pieceDesGridPos.x, pieceDesGridPos.y);

	// Coronación
	if (this->isAIMove && this->movingPiece->getType() == PieceType::PEON && (pieceDesGridPos.x == 0 || pieceDesGridPos.x == 7)) {
		this->promotion(turn, pieceDesGridPos, true); // Aplicar directamente
	} else {
		this->promotion(turn, pieceDesGridPos, false);
	}

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

	// Efectos de sonido
	if (this->status != GameStatus::PLAYING) {
		AudioSystem::getInstance().playSound("game_over");
	} else if (this->jaque[turn]) {
		AudioSystem::getInstance().playSound("check");
	} else if (menacedPiece != nullptr || (this->movingPiece->getType() == PieceType::PEON && pieceStartGridPos.y != pieceDesGridPos.y && this->board[pieceDesGridPos.x][pieceDesGridPos.y] == (turn ? "PN" : "PB"))) { 
		// if en passant happened, menacedPiece might be null but we captured something
		AudioSystem::getInstance().playSound("capture");
	} else {
		AudioSystem::getInstance().playSound("move");
	}

	record.isCheck = this->jaque[turn];
	record.isCheckmate = (this->status == GameStatus::CHECKMATE);
	this->moveHistory.addMove(record);
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

		// Comprobamos jaque al oponente después de la promoción
		bool opponentTurn = !turn;
		this->jaque[0] = false;
		this->jaque[1] = false;

		if (isInCheck(opponentTurn, this->board)) {
			this->jaque[opponentTurn] = true;
			Piece* king = this->pieces[7][opponentTurn].get();
			this->jaqueCell.setPosition(king->getPosition().x, king->getPosition().y);

			if (isCheckmate(opponentTurn)) {
				this->status = GameStatus::CHECKMATE;
			}
		} else {
			if (isCheckmate(opponentTurn)) {
				this->status = GameStatus::STALEMATE;
			}
		}

		// Efectos de sonido post-promoción
		if (this->status != GameStatus::PLAYING) {
			AudioSystem::getInstance().playSound("game_over");
		} else if (this->jaque[opponentTurn]) {
			AudioSystem::getInstance().playSound("check");
		}

		if (this->moveHistory.size() > 0) {
			MoveRecord& lastMove = this->moveHistory.getLastMoveRef();
			lastMove.promotedTo = this->promotionPiece;
			lastMove.isCheck = this->jaque[opponentTurn];
			lastMove.isCheckmate = (this->status == GameStatus::CHECKMATE);
		}
	}
	else {
		if (this->movingPiece->getType() == PieceType::PEON) {

			if (gridPos.x == 0 || gridPos.x == 7) {
				float squareY = rowToPixelY(gridPos.x);
				float menuY = (squareY < BOARD_SIZE / 2) ? squareY + CELL_SIZE : squareY - 2 * CELL_SIZE;
				this->promotionMenu->setPosition(colToPixelX(gridPos.y) + (CELL_SIZE - 70.f) / 2.f, menuY);
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

	if (this->peonPiece) {
		this->peonPiece->setActive(false);
		this->peonPiece->move(-100, -100);
	}
}


Board::Board(std::map<std::string, sf::Texture>& textures) :
	promotionQB(textures["QB"]), promotionQN(textures["QN"]),
	promotionCB(textures["CB"]), promotionCN(textures["CN"]),
	promotionAB(textures["AB"]), promotionAN(textures["AN"]),
	promotionTB(textures["TB"]), promotionTN(textures["TN"]) {

	this->initTextures(textures);
	this->initVariables();
	this->initPieces(textures);
	this->initCoordinates();
}

void Board::initCoordinates() {
	if (!this->coordFont.loadFromFile("resources/fonts/Factory LJDS.ttf")) {
		throw std::runtime_error("ERROR::BOARD::COULD NOT LOAD COORD FONT");
	}

	this->coordLabels.clear();
	const unsigned charSize = 16;
	const sf::Color fill(245, 240, 235, 255);
	const sf::Color outline(60, 45, 40, 255);

	auto makeLabel = [&](const std::string& str) {
		sf::Text t;
		t.setFont(this->coordFont);
		t.setCharacterSize(charSize);
		t.setString(str);
		t.setFillColor(fill);
		t.setOutlineColor(outline);
		t.setOutlineThickness(1.5f);
		return t;
	};

	// Letras de columna (a-h) en el margen inferior. colToPixelX ya aplica el volteo.
	for (int col = 0; col < 8; ++col) {
		sf::Text t = makeLabel(std::string(1, static_cast<char>('a' + col)));
		sf::FloatRect b = t.getLocalBounds();
		float cx = colToPixelX(col) + CELL_SIZE / 2.f;
		t.setPosition(cx - b.left - b.width / 2.f, 800.f);
		this->coordLabels.push_back(t);
	}

	// Números de fila (1-8) en el margen izquierdo. La fila r corresponde al rango (8 - r).
	for (int r = 0; r < 8; ++r) {
		sf::Text t = makeLabel(std::to_string(8 - r));
		sf::FloatRect b = t.getLocalBounds();
		float cy = rowToPixelY(r) + CELL_SIZE / 2.f;
		t.setPosition(4.f - b.left - b.width / 2.f, cy - b.top - b.height / 2.f);
		this->coordLabels.push_back(t);
	}
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

bool Board::checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, Piece* movingPiece, Piece* menacedPiece, CastlingState& castling, BoardGrid& checkBoard, EnPassantState& peonPaso) {
	if ((turn == movingPiece->getColor())&&(startPos!=desPos)) {
		if (menacedPiece) {
			if(menacedPiece->getColor() != movingPiece->getColor() && menacedPiece->isActive()) {
				if (movingPiece->checkMove(turn, startPos, desPos, checkBoard, castling, peonPaso)) {
					return true;
				}
			}
		}
		else {
			if (movingPiece->checkMove(turn, startPos, desPos, checkBoard, castling, peonPaso)) {
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

sf::Vector2i Board::mouseToGrid(sf::Vector2i mousePos) const {
	// x = fila (desde mousePos.y), y = columna (desde mousePos.x)
	return sf::Vector2i(pixelYToRow(mousePos.y), pixelXToCol(mousePos.x));
}

bool Board::isInsideBoard(sf::Vector2i mousePos) const {
	return mousePos.x > BOARD_OFFSET_X && mousePos.x < BOARD_OFFSET_X + BOARD_SIZE
	    && mousePos.y > BOARD_OFFSET_Y && mousePos.y < BOARD_OFFSET_Y + BOARD_SIZE;
}

void Board::onPress(sf::Vector2i mousePos, bool& turn, int& points1, int& points2) {
	if (!this->isInsideBoard(mousePos)) return;
	sf::Vector2i sq = this->mouseToGrid(mousePos);

	if (this->isMoving && this->movingPiece) {
		sf::Vector2i selected = this->movingPiece->getGridPosition();
		if (sq == selected) {
			// Volver a agarrar la pieza ya seleccionada para arrastrarla.
			this->isDragging = true;
		} else {
			Piece* atSquare = this->getPiece(sq.x, sq.y);
			if (atSquare && atSquare->isActive() && atSquare->getColor() == turn) {
				// Cambiar la selección a otra pieza propia (y permitir arrastrarla).
				this->startMove(mousePos, turn);
				if (this->isMoving) this->isDragging = true;
			} else {
				// Clic en una casilla destino (modo dos clics).
				this->endMove(mousePos, turn, points1, points2);
			}
		}
	} else {
		// Nada seleccionado: intentar seleccionar la pieza bajo el cursor.
		this->startMove(mousePos, turn);
		if (this->isMoving) this->isDragging = true;
	}
}

void Board::onDrag(sf::Vector2i mousePos) {
	if (this->isDragging && this->movingPiece) {
		// Centrar el sprite de la pieza en el cursor.
		this->movingPiece->setRenderPosition(mousePos.x - CELL_SIZE / 2.f, mousePos.y - CELL_SIZE / 2.f);
	}
}

void Board::onRelease(sf::Vector2i mousePos, bool& turn, int& points1, int& points2) {
	if (!this->isDragging || !this->movingPiece) {
		this->isDragging = false;
		return;
	}
	this->isDragging = false;

	Piece* dragged = this->movingPiece;
	sf::Vector2i origin = dragged->getGridPosition();

	// Soltar fuera del tablero o en la misma casilla no es un arrastre: se devuelve
	// el sprite a su sitio y se mantiene la selección (para completar por dos clics).
	if (!this->isInsideBoard(mousePos) || this->mouseToGrid(mousePos) == origin) {
		dragged->snapToGrid();
		return;
	}

	// Arrastre a otra casilla: intentar el movimiento.
	this->endMove(mousePos, turn, points1, points2);

	// Si el movimiento no se aplicó (ilegal), el sprite quedó bajo el cursor: devolverlo.
	if (dragged->getGridPosition() == origin) {
		dragged->snapToGrid();
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

		EnPassantState testPeonPaso = this->peonPaso;
		if (this->checkMove(turn, attackerPos, targetPos, attacker, targetPiece, castling, board, testPeonPaso))
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

bool Board::isInCheck(bool color) {
	return this->isInCheck(color, this->board);
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
                EnPassantState testPeonPaso = this->peonPaso;

                if (p->checkMove(color, startPos, desPos, testBoard, testCastling, testPeonPaso)) {
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
	// Las piezas ahora se actualizan en updateAnimations(float dt)




	// Esto esta MUY RARO. Para coronar utiliza posicion del raton calculada en State.cpp. Para mover el juego utiliza la posicion interna del Board
	if (!this->promotionMenu->isShown()) {

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			if (this->isDragging && this->movingPiece) this->movingPiece->snapToGrid();
			this->isMoving = false;
			this->isDragging = false;
			this->legalMovesShapes.clear();
		}
		this->mousePos = sf::Mouse::getPosition(window);
	}
	else {
		this->promotionMenu->update(mousePos);
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

	// Coordenadas (a-h, 1-8) dibujadas dinámicamente para respetar la orientación.
	for (const sf::Text& label : this->coordLabels) {
		target.draw(label);
	}

	// Último movimiento
	if (this->hasLastMove) {
		target.draw(this->lastMoveStartCell);
		target.draw(this->lastMoveEndCell);
	}

	// Casilla selecionada
	if (this->isMoving) target.draw(this->selectedCell);

	// Jaques
	if (this->jaque[0] || this->jaque[1]) target.draw(this->jaqueCell);

	// Piezas
	for (int i = 0; i<16; i++) {
		if (this->pieces[i][0]->isActive()) this->pieces[i][0]->render(target);
		if (this->pieces[i][1]->isActive()) this->pieces[i][1]->render(target);
	}

	// Movimientos legales
	if (this->isMoving) {
		for (auto& shape : this->legalMovesShapes) {
			target.draw(shape);
		}
	}

	// La pieza que se está arrastrando se dibuja por encima del resto.
	if (this->isDragging && this->movingPiece) this->movingPiece->render(target);

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

bool Board::isPlaying() const {
	return this->status == GameStatus::PLAYING;
}

bool Board::isAnyPieceAnimating() const {
	for (int i = 0; i < 16; i++) {
		if (this->pieces[i][0] && this->pieces[i][0]->isActive() && this->pieces[i][0]->getIsAnimating()) return true;
		if (this->pieces[i][1] && this->pieces[i][1]->isActive() && this->pieces[i][1]->getIsAnimating()) return true;
	}
	return false;
}

void Board::updateAnimations(float dt) {
	for (int i = 0; i < 16; i++) {
		if (this->pieces[i][0] && this->pieces[i][0]->isActive()) this->pieces[i][0]->update(dt);
		if (this->pieces[i][1] && this->pieces[i][1]->isActive()) this->pieces[i][1]->update(dt);
	}
}

bool Board::isMoveLegal(bool turn, sf::Vector2i from, sf::Vector2i to) const {
    Piece* p = const_cast<Board*>(this)->getPiece(from.x, from.y);
    if (!p || p->getColor() != turn) return false;
    
    Piece* targetPiece = const_cast<Board*>(this)->getPiece(to.x, to.y);
    
    BoardGrid testBoard = this->board;
    CastlingState testCastling = this->castling;
    EnPassantState testPeonPaso = this->peonPaso;
    
    if (!const_cast<Board*>(this)->checkMove(turn, from, to, p, targetPiece, testCastling, testBoard, testPeonPaso)) {
        return false;
    }
    
    bool isCastling = (p->getType() == PieceType::REY && std::abs(from.y - to.y) == 2);
    if (isCastling) {
        if (!const_cast<Board*>(this)->isCastlingLegal(turn, from, to, const_cast<BoardGrid&>(this->board))) {
            return false;
        }
    }
    
    testBoard[to.x][to.y] = testBoard[from.x][from.y];
    if ((from.x + from.y) % 2 != 0)
        testBoard[from.x][from.y] = "-";
    else
        testBoard[from.x][from.y] = "+";
        
    if (testPeonPaso[to.y][!turn] && from.y != to.y && p->getType() == PieceType::PEON) {
        int capRow = turn ? 3 : 4;
        if ((capRow + to.y) % 2 != 0)
            testBoard[capRow][to.y] = "-";
        else
            testBoard[capRow][to.y] = "+";
    }
        
    if (const_cast<Board*>(this)->isInCheck(turn, testBoard)) {
        return false;
    }
    
    return true;
}

bool Board::needsDisambiguation(bool turn, PieceType type, sf::Vector2i from, sf::Vector2i to) const {
    if (type == PieceType::PEON || type == PieceType::REY) return false;
    for (int i = 0; i < 16; ++i) {
        Piece* p2 = pieces[i][turn].get();
        if (p2 && p2->isActive() && p2->getType() == type) {
            sf::Vector2i p2Pos = p2->getGridPosition();
            if (p2Pos != from) {
                if (isMoveLegal(turn, p2Pos, to)) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string Board::getTextureKey(PieceType type, bool color) {
    std::string key = "";
    switch (type) {
        case PieceType::PEON: key = "P"; break;
        case PieceType::TORRE: key = "T"; break;
        case PieceType::CABALLO: key = "C"; break;
        case PieceType::ALFIL: key = "A"; break;
        case PieceType::REINA: key = "Q"; break;
        case PieceType::REY: key = "K"; break;
    }
    key += color ? "B" : "N";
    return key;
}

GameSnapshot Board::captureSnapshot() const {
    GameSnapshot snap;
    snap.board = this->board;
    for (int i = 0; i < 16; ++i) {
        for (int c = 0; c < 2; ++c) {
            Piece* p = pieces[i][c].get();
            int index = i * 2 + c;
            snap.pieces[index].gridPos = p->getGridPosition();
            snap.pieces[index].active = p->isActive();
            snap.pieces[index].type = p->getType();
            snap.pieces[index].color = p->getColor();
        }
    }
    snap.castling = this->castling;
    snap.peonPaso = this->peonPaso;
    snap.jaque = this->jaque;
    snap.status = this->status;
    snap.halfMoveClock = this->halfMoveClock;
    snap.positionHistory = this->positionHistory;
    snap.moveHistory = this->moveHistory;
    
    // UI Visuals
    snap.hasLastMove = this->hasLastMove;
    snap.lastMoveStartPos = this->lastMoveStartCell.getPosition();
    snap.lastMoveEndPos = this->lastMoveEndCell.getPosition();
    return snap;
}

void Board::restoreSnapshot(const GameSnapshot& snap, std::map<std::string, sf::Texture>& textures) {
    this->board = snap.board;
    for (int i = 0; i < 16; ++i) {
        for (int c = 0; c < 2; ++c) {
            int index = i * 2 + c;
            Piece* p = pieces[i][c].get();
            const auto& pSnap = snap.pieces[index];
            
            p->setActive(pSnap.active);
            if (pSnap.active) {
                p->setGridPositionImmediate(pSnap.gridPos.x, pSnap.gridPos.y);
            } else {
                p->setGridPositionImmediate(-100, -100);
            }
            
            if (p->getType() != pSnap.type) {
                p->setPieceType(pSnap.type);
                p->setTexture(textures[getTextureKey(pSnap.type, pSnap.color)]);
            }
        }
    }
    this->castling = snap.castling;
    this->peonPaso = snap.peonPaso;
    this->jaque = snap.jaque;
    this->status = snap.status;
    this->halfMoveClock = snap.halfMoveClock;
    this->positionHistory = snap.positionHistory;
    this->moveHistory = snap.moveHistory;
    
    // UI Visuals
    this->hasLastMove = snap.hasLastMove;
    if (this->hasLastMove) {
        this->lastMoveStartCell.setPosition(snap.lastMoveStartPos);
        this->lastMoveEndCell.setPosition(snap.lastMoveEndPos);
    }
    
    // Reset any moving state
    this->isMoving = false;
    this->isDragging = false;
    this->legalMovesShapes.clear();
    this->movingPiece = nullptr;
    this->menacedPiece = nullptr;
    this->peonPiece = nullptr;
}

bool Board::applyAIMove(int fromX, int fromY, int toX, int toY, PieceType promotion, bool& turn, int& points1, int& points2) {
	Piece* p = getPiece(fromX, fromY);
	if (!p || p->getColor() != turn) return false;

	this->movingPiece = p;
	this->isMoving = true;
	this->legalMovesShapes.clear();
	this->calculateLegalMoves(turn, sf::Vector2i(fromX, fromY));

	Piece* targetPiece = getPiece(toX, toY);
	if (!targetPiece && p->getType() == PieceType::PEON && fromY != toY) {
		targetPiece = getPiece(fromX, toY); // En passant
	}
	
	this->menacedPiece = targetPiece;
	this->isAIMove = true;
	this->promotionPiece = promotion;
	
	sf::Vector2i pixelPos(static_cast<int>(colToPixelX(toY)) + CELL_SIZE/2, static_cast<int>(rowToPixelY(toX)) + CELL_SIZE/2);
	endMove(pixelPos, turn, points1, points2);
	
	this->isAIMove = false;
	return true;
}

std::vector<Piece*> Board::getCapturedPieces(bool color) const {
	std::vector<Piece*> captured;
	int colIdx = color ? 1 : 0;
	for (int i = 0; i < 16; i++) {
		Piece* p = this->pieces[i][colIdx].get();
		if (p && !p->isActive() && p->getType() != PieceType::REY) {
			captured.push_back(p);
		}
	}
	return captured;
}

