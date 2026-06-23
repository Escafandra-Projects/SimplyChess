#include "chess/Piece.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>

Piece::Piece() {
	this->points = 0;
	this->color = true;
	this->type = PieceType::PEON;
	this->active = true;
	this->isAnimating = false;
}

Piece::Piece(unsigned x, unsigned y, sf::Texture& texture, PieceType type, bool color) {
	this->type = type;
	this->color = color;
	this->active = true;
	this->points = 0;

	this->gridPos.y = y;
	this->gridPos.x = x;
	this->pos.x = this->gridPos.y * CELL_SIZE + BOARD_OFFSET_X;
	this->pos.y = this->gridPos.x * CELL_SIZE + BOARD_OFFSET_Y;
	this->targetPos = this->pos;
	this->isAnimating = false;
	this->piece.setTexture(texture);
	this->piece.setScale(0.95f, 0.95f);
	this->piece.setPosition(pos.x, pos.y);


	switch (type) {
	case PieceType::PEON:
		this->points = 1;
		break;
	case PieceType::CABALLO:
		this->points = 3;
		break;
	case PieceType::ALFIL:
		this->points = 3;
		break;
	case PieceType::TORRE:
		this->points = 5;
		break;
	case PieceType::REINA:
		this->points = 9;
		break;
	case PieceType::REY:
		this->points = 0;
		break;
	}

}

Piece::~Piece() {

}

void Piece::update(float dt) {
	if (isAnimating) {
		float dx = targetPos.x - pos.x;
		float dy = targetPos.y - pos.y;
		float dist = std::sqrt(dx * dx + dy * dy);
		
		if (dist < 1.0f) {
			pos = targetPos;
			isAnimating = false;
		} else {
			pos.x += dx * ANIMATION_SPEED * dt;
			pos.y += dy * ANIMATION_SPEED * dt;
		}
		this->piece.setPosition(pos.x, pos.y);
	}
}

void Piece::render(sf::RenderTarget& target) {
	target.draw(piece);
}

void Piece::setActive(bool isActive) {
	this->active = isActive;
}

void Piece::setColor(bool color){
	this->color = color;
}

void Piece::setPieceType(PieceType type) {
	this->type = type;
	switch (type)
	{
		case PieceType::REINA:
			this->points = 9;
			break;
		case PieceType::CABALLO:
			this->points = 3;
			break;
		case PieceType::ALFIL:
			this->points = 3;
			break;
		case PieceType::TORRE:
			this->points = 5;
			break;
		default:
			break;
	}
}
void Piece::setTexture(sf::Texture& texture){
	this->piece.setTexture(texture);
}

bool Piece::getColor() const {
	return this->color;
}

PieceType Piece::getType() const {
	return this->type;
}

sf::Vector2i Piece::getPosition() const {
	return static_cast<sf::Vector2i>(this->piece.getPosition());
}

sf::Vector2i Piece::getGridPosition() const {
	return sf::Vector2i(this->gridPos.x, this->gridPos.y);
}

int Piece::getPoints() const {
	return this->points;
}

bool Piece::isActive() const {
    return this->active;
}

bool Piece::getIsAnimating() const {
	return this->isAnimating;
}

void Piece::move(int x, int y){
	this->targetPos.x = y * CELL_SIZE + BOARD_OFFSET_X;
	this->targetPos.y = x * CELL_SIZE + BOARD_OFFSET_Y;
	this->gridPos.x = x;
	this->gridPos.y = y;

	this->isAnimating = true;
}

void Piece::setGridPositionImmediate(unsigned x, unsigned y) {
	this->gridPos.x = x;
	this->gridPos.y = y;
	this->pos.x = y * CELL_SIZE + BOARD_OFFSET_X;
	this->pos.y = x * CELL_SIZE + BOARD_OFFSET_Y;
	this->targetPos = this->pos;
	this->piece.setPosition(this->pos);
	this->isAnimating = false;
}

void Piece::setRenderPosition(float x, float y){
	// Mueve solo el sprite (arrastre): sin animación y sin tocar la casilla lógica.
	this->pos.x = x;
	this->pos.y = y;
	this->targetPos = this->pos;
	this->isAnimating = false;
	this->piece.setPosition(this->pos.x, this->pos.y);
}

void Piece::snapToGrid(){
	// Recoloca el sprite en su casilla de rejilla actual, sin animación.
	this->pos.x = this->gridPos.y * CELL_SIZE + BOARD_OFFSET_X;
	this->pos.y = this->gridPos.x * CELL_SIZE + BOARD_OFFSET_Y;
	this->targetPos = this->pos;
	this->isAnimating = false;
	this->piece.setPosition(this->pos.x, this->pos.y);
}

bool Piece::checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, CastlingState& castling, EnPassantState& peonPaso) {
	switch (this->type) {
	case PieceType::PEON:
		return this->checkMovePeon(turn, startPos, desPos, board, peonPaso);

	case PieceType::CABALLO:
		return this->checkMoveCaballo(turn, startPos, desPos, board);

	case PieceType::ALFIL:
		return this->checkMoveAlfil(turn, startPos, desPos, board);

	case PieceType::TORRE:
		return this->checkMoveTorre(turn, startPos, desPos, board);

	case PieceType::REINA:
		return this->checkMoveQueen(turn, startPos, desPos, board);

	case PieceType::REY:
		return this->checkMoveKing(turn, startPos, desPos, board, castling);

	}

	return false;
}

bool Piece::checkMoveKing(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, CastlingState& castling) {
	bool success = false;


	if (std::abs(desPos.x - startPos.x) <= 1 && std::abs(desPos.y - startPos.y) <= 1) {
		success = true;
		castling[4] = false;
	}
	else if(castling[4]) {
		//Reglas del enroque
		if (turn) {
			if (board[7][4] == "KB") {
				if (castling[0] || castling[1]) {
					//Enroque corto blancas
					if ((board[7][6] == "+" || board[7][6] == "-") && (board[7][5] == "+" || board[7][5] == "-")) {
						if (desPos.x == 7 && desPos.y == 6) {
							board[7][7] = "+";
							board[7][5] = "TB";
							success = true;
							castling[0] = false;
						}
					}

					//Enroque largo blancas
					if ((board[7][1] == "+" || board[7][1] == "-") && (board[7][2] == "+" || board[7][2] == "-") && (board[7][3] == "+" || board[7][3] == "-")) {
						if (desPos.x == 7 && desPos.y == 2) {
							board[7][0] = "+";
							board[7][3] = "TB";
							success = true;
							castling[1] = false;
						}
					}
				}
			}
		}
		else {
			if (board[0][4] == "KN") {
				if (castling[2] || castling[3]) {
					//Enroque corto negras
					if ((board[0][6] == "+" || board[0][6] == "-") && (board[0][5] == "+" || board[0][5] == "-")) {
						if (desPos.x == 0 && desPos.y == 6) {
							board[0][7] = "-";
							board[0][5] = "TN";
							success = true;
							castling[2] = false;
						}
					}

					//Enroque largo negras
					if ((board[0][1] == "+" || board[0][1] == "-") && (board[0][2] == "+" || board[0][2] == "-") && (board[0][3] == "+" || board[0][3] == "-")) {
						if (desPos.x == 0 && desPos.y == 2) {
							board[0][0] = "-";
							board[0][3] = "TN";
							success = true;
							castling[3] = false;
						}
					}
				}
			}
		}
	}
	return success;
}


bool Piece::checkMoveQueen(bool /*turn*/, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board) {
	bool success = true;
	if ((startPos.x == desPos.x || startPos.y == desPos.y) || (std::abs(desPos.x - startPos.x) == std::abs(desPos.y - startPos.y))) {

		if (startPos.x == desPos.x) {
			for (int i = std::min(startPos.y, desPos.y) + 1; i < std::max(startPos.y, desPos.y); i++) {
				if (board[startPos.x][i] != "+" && board[startPos.x][i] != "-") success = false;
			}
		}
		else if (startPos.y == desPos.y) {
			for (int i = std::min(startPos.x, desPos.x) + 1; i < std::max(startPos.x, desPos.x); i++) {
				if (board[i][startPos.y] != "+" && board[i][startPos.y] != "-") success = false;
			}
		}
		else {
			//Distingo casos (horrible e ineficiente, pero seguro)
			if (desPos.x > startPos.x && desPos.y > startPos.y) {
				for (int i = 1; i < desPos.x - startPos.x; i++) {
					if (board[startPos.x + i][startPos.y + i] != "+" && board[startPos.x + i][startPos.y + i] != "-") success = false;
				}
			}
			else if (desPos.x<startPos.x && desPos.y>startPos.y) {
				for (int i = 1; i < startPos.x - desPos.x; i++) {
					if (board[startPos.x - i][startPos.y + i] != "+" && board[startPos.x - i][startPos.y + i] != "-") success =  false;
				}
			}
			else if (desPos.x > startPos.x && desPos.y < startPos.y) {
				for (int i = 1; i < desPos.x - startPos.x; i++) {
					if (board[startPos.x + i][startPos.y - i] != "+" && board[startPos.x + i][startPos.y - i] != "-") success = false;
				}
			}
			else {
				for (int i = 1; i < startPos.x - desPos.x; i++) {
					if (board[startPos.x - i][startPos.y - i] != "+" && board[startPos.x - i][startPos.y - i] != "-") success = false;
				}
			}
		}
	}
	else success = false;

	return success;
}

bool Piece::checkMoveTorre(bool /*turn*/, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board) {
	bool success = true;

	if ((startPos.x == desPos.x) || (startPos.y == desPos.y)) {

		if (startPos.x == desPos.x) {
			for (int i = std::min(startPos.y, desPos.y) + 1; i < std::max(startPos.y, desPos.y); i++) {

				if (board[startPos.x][i] != "+" && board[startPos.x][i] != "-") success = false;

			}
		}
		else {
			for (int i = std::min(startPos.x, desPos.x) + 1; i < std::max(startPos.x, desPos.x); i++) {

				if (board[i][startPos.y] != "+" && board[i][startPos.y] != "-") success = false;

			}
		}
	}
	else success = false;

	return success;
}

bool Piece::checkMoveAlfil(bool /*turn*/, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board){
	bool success = true;
	if (std::abs(desPos.x - startPos.x) == std::abs(desPos.y - startPos.y)) {

		//Distingo casos (horrible e ineficiente, pero seguro)
		if (desPos.x > startPos.x && desPos.y > startPos.y) {
			for (int i = 1; i < desPos.x - startPos.x; i++) {
				if (board[startPos.x + i][startPos.y + i] != "+" && board[startPos.x + i][startPos.y + i] != "-") success = false;
			}
		}
		else if (desPos.x < startPos.x && desPos.y > startPos.y) {
			for (int i = 1; i < startPos.x - desPos.x; i++) {
				if (board[startPos.x - i][startPos.y + i] != "+" && board[startPos.x - i][startPos.y + i] != "-") success = false;
			}
		}
		else if (desPos.x > startPos.x && desPos.y < startPos.y) {
			for (int i = 1; i < desPos.x - startPos.x; i++) {
				if (board[startPos.x + i][startPos.y - i] != "+" && board[startPos.x + i][startPos.y - i] != "-") success = false;
			}
		}
		else {
			for (int i = 1; i < startPos.x - desPos.x; i++) {
				if (board[startPos.x - i][startPos.y - i] != "+" && board[startPos.x - i][startPos.y - i] != "-") success = false;
			}
		}
	}
	else success = false;

	return success;

}

bool Piece::checkMoveCaballo(bool /*turn*/, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& /*board*/) {
	if ((std::abs((std::abs(startPos.x - desPos.x) - std::abs(startPos.y - desPos.y))) == 1)
		&& ((std::abs(desPos.x - startPos.x) == 2
			&& std::abs(desPos.y - startPos.y) == 1)
			|| (std::abs(desPos.x - startPos.x) == 1
				&& std::abs(desPos.y - startPos.y) == 2))) return true;

	return false;

}

bool Piece::checkMovePeon(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, EnPassantState& peonPaso) {

	bool success = false;

		//El movimiento del peon depende de si es turno de blancas o negras
                if(turn) {
                    if(startPos.y==desPos.y) {
                        //1 casilla adelante
                        if((desPos.x==startPos.x-1)&&(board[desPos.x][desPos.y]=="+"||board[desPos.x][desPos.y]=="-")) success = true;
                        //2 casillas adelante
						if (startPos.x == 6 && desPos.x == startPos.x - 2 && (board[desPos.x][desPos.y] == "+" || board[desPos.x][desPos.y] == "-")) {
							success = true;
							//Tenemos en cuenta para peon al paso
							peonPaso[startPos.y][1] = true;
						}

                    }
                    else {
                        //Diagonal y comer
						if (std::abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x - 1 && board[desPos.x][desPos.y] != "+" && board[desPos.x][desPos.y] != "-") {
							success = true;
						}
						//Posibilidad de comer al paso
						else {
							if (std::abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x - 1 && peonPaso[desPos.y][0] == true && startPos.x==3) {
								success = true;
							}
						}

					}

                }
				else {
					if (startPos.y == desPos.y) {
						//1 casilla adelante
						if ((desPos.x == startPos.x + 1) && (board[desPos.x][desPos.y] == "+" || board[desPos.x][desPos.y] == "-")) success = true;
						//2 casillas adelante
						if (startPos.x == 1 && desPos.x == startPos.x + 2 && (board[desPos.x][desPos.y] == "+" || board[desPos.x][desPos.y] == "-")) {
							success = true;
							// Para el peon al paso
							peonPaso[startPos.y][0] = true;
						}

					}
					else {
						//Diagonal y comer
						if (std::abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x + 1 && board[desPos.x][desPos.y] != "+" && board[desPos.x][desPos.y] != "-") {
							success = true;
						}
						else {
						//Posibilidad de comer al paso
							if (std::abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x + 1 && peonPaso[desPos.y][1] == true && startPos.x==4) {
								success = true;
							}
						}
					}


				}



	return success;
}
