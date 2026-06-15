#pragma once

#include <SFML/Graphics.hpp>
#include <iostream> 
#include <algorithm>

using namespace std;

enum PieceType { PEON = 0, TORRE, CABALLO, ALFIL, REINA, REY };

class Piece {
private:
    sf::Sprite piece;     
    bool selected;
    bool color; // 1 blanca, 0 negra
    bool active;
    unsigned short type;
    unsigned short points;
    sf::Vector2f pos;
    sf::Vector2u gridPos;

    
    bool checkMoveKing(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8], bool castling[5]);
    bool checkMoveQueen(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]);
    bool checkMoveTorre(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]);
    bool checkMoveAlfil(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]);
    bool checkMoveCaballo(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8]);
    bool checkMovePeon(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8], bool peonPaso[8][2]);
    
public:
    //Parámetros de la pieza la posición inicial, su textura y puntuación
    Piece();
    Piece(unsigned x, unsigned y, sf::Texture& texture, unsigned short type, bool color);
    virtual ~Piece();

    void update();
    void render(sf::RenderTarget& target);

    void setActive(bool isActive);
    void setColor(bool color);
    void setPieceType(unsigned short type);
    void setTexture(sf::Texture& texture);

    bool getColor();
    sf::Vector2i getPosition();
    sf::Vector2i getGridPosition();
    int getPoints();
    bool isActive();

    void move(int x, int y);

    bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, string board[8][8], bool castling[5], bool peonPaso[8][2]);


    
};
    

