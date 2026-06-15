#include "Piece.h"


Piece::Piece() {
	this->selected = false;
	this->points = 0;
	this->color = true;
	this->type = 0;
	this->active = true;

}

Piece::Piece(unsigned x, unsigned y, sf::Texture& texture, unsigned short type, bool color) {
	this->selected = false;
	this->type = type;
	this->color = color;
	this->active = true;
	this->points = 0;

	this->gridPos.y = y;
	this->gridPos.x = x;
	this->pos.x = this->gridPos.y * 100;
	this->pos.y = this->gridPos.x * 100;
	this->piece.setTexture(texture);
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
   
void Piece::update() {
	
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

void Piece::setPieceType(unsigned short type) {
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
	}
}
void Piece::setTexture(sf::Texture& texture){
	this->piece.setTexture(texture);
}

bool Piece::getColor() {
	return this->color;
}
 
sf::Vector2i Piece::getPosition(){
	return static_cast<sf::Vector2i>(this->piece.getPosition());
}

sf::Vector2i Piece::getGridPosition() {
	return sf::Vector2i(this->gridPos.x, this->gridPos.y);
}

int Piece::getPoints() {
	return this->points;
}

bool Piece::isActive(){
    return this->active;
}


void Piece::move(int x, int y){
	this->pos.x = y * 100;
	this->pos.y = x * 100;
	this->gridPos.x = x;
	this->gridPos.y = y;

	this->piece.setPosition(this->pos.x, this->pos.y);
}

bool Piece::checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8], bool castling[5], bool peonPaso[8][2]) {
	switch (this->type) {
	case PieceType::PEON:
		std::cout << "compruebo movimiento del peon" << std::endl;
		return this->checkMovePeon(turn, startPos, desPos, board, peonPaso);
		
	case PieceType::CABALLO:
		std::cout << "compruebo movimiento del caballo" << std::endl;
		return this->checkMoveCaballo(turn, startPos, desPos, board);
		
	case PieceType::ALFIL:
		std::cout << "compruebo movimiento del alfil" << std::endl;
		return this->checkMoveAlfil(turn, startPos, desPos, board);
		
	case PieceType::TORRE:
		std::cout << "compruebo movimiento del torre" << std::endl;
		return this->checkMoveTorre(turn, startPos, desPos, board);
	case PieceType::REINA:
		std::cout << "compruebo movimiento del reina" << std::endl;
		return this->checkMoveQueen(turn, startPos, desPos, board);
		
	case PieceType::REY:
		std::cout << "compruebo movimiento del rey" << std::endl;
		return this->checkMoveKing(turn, startPos, desPos, board, castling);
		
	}

	return false;
}

bool Piece::checkMoveKing(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8], bool castling[5]) {
	bool success = false;
	

	if (abs(desPos.x - startPos.x) <= 1 && abs(desPos.y - startPos.y) <= 1) {
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


bool Piece::checkMoveQueen(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]) {
	bool success = true;
	if ((startPos.x == desPos.x || startPos.y == desPos.y) || (abs(desPos.x - startPos.x) == abs(desPos.y - startPos.y))) {

		if (startPos.x == desPos.x) {
			for (int i = min(startPos.y, desPos.y) + 1; i < max(startPos.y, desPos.y); i++) {
				if (board[startPos.x][i] != "+" && board[startPos.x][i] != "-") success = false;
			}
		}
		else if (startPos.y == desPos.y) {
			for (int i = min(startPos.x, desPos.x) + 1; i < max(startPos.x, desPos.x); i++) {
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

bool Piece::checkMoveTorre(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]) {
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

bool Piece::checkMoveAlfil(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]){
	
	bool success = true;
	if (abs(desPos.x - startPos.x) == abs(desPos.y - startPos.y)) {
	    
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

bool Piece::checkMoveCaballo(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]) {
	if ((abs((abs(startPos.x - desPos.x) - abs(startPos.y - desPos.y))) == 1) 
		&& ((abs(desPos.x - startPos.x) == 2 
			&& abs(desPos.y - startPos.y) == 1) 
			|| (abs(desPos.x - startPos.x) == 1 
				&& abs(desPos.y - startPos.y) == 2))) return true;
	
	return false;

}

bool Piece::checkMovePeon(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8], bool peonPaso[8][2]) {

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
						if (abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x - 1 && board[desPos.x][desPos.y] != "+" && board[desPos.x][desPos.y] != "-") {
							success = true;
						}
						//Posibilidad de comer al paso
						else {
							if (abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x - 1 && peonPaso[desPos.y][0] == true && startPos.x==3) {
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
						if (abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x + 1 && board[desPos.x][desPos.y] != "+" && board[desPos.x][desPos.y] != "-") {
							success = true;
						}
						else {
						//Posibilidad de comer al paso
							if (abs(startPos.y - desPos.y) == 1 && desPos.x == startPos.x + 1 && peonPaso[desPos.y][1] == true && startPos.x==4) {
								success = true;
							}
						}
					}

					
				}
            


	
	return success;
}

