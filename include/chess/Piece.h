#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

// Lado de cada casilla del tablero, en píxeles.
constexpr int CELL_SIZE = 100;

// Origen de la rejilla jugable dentro de la imagen del tablero, en píxeles.
// La franja con los números (1-8) ocupa el margen izquierdo, por lo que la
// rejilla queda desplazada hacia la derecha respecto a la esquina de la imagen.
constexpr int BOARD_OFFSET_X = 20;
constexpr int BOARD_OFFSET_Y = 0;

// Tipo de pieza. El valor subyacente coincide con el orden histórico del proyecto.
enum class PieceType : unsigned short { PEON = 0, TORRE, CABALLO, ALFIL, REINA, REY };

// Alias del modelo de datos del tablero, compartidos por Piece y Board.
// El tablero usa códigos de 2 chars ("PB", "TN"...) y "+"/"-" para casillas vacías.
using BoardGrid      = std::array<std::array<std::string, 8>, 8>;
using CastlingState  = std::array<bool, 5>;
using EnPassantState = std::array<std::array<bool, 2>, 8>;

/// Una pieza del tablero: conoce su tipo, color, posición y sabe validar
/// sus propios movimientos según las reglas del ajedrez.
class Piece {
private:
    sf::Sprite piece;
    bool color; // 1 blanca, 0 negra
    bool active;
    PieceType type;
    unsigned short points;
    sf::Vector2f pos;
    sf::Vector2u gridPos;


    /// Valida el movimiento del rey (incluye el enroque).
    bool checkMoveKing(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, CastlingState& castling);
    /// Valida el movimiento de la dama.
    bool checkMoveQueen(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board);
    /// Valida el movimiento de la torre.
    bool checkMoveTorre(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board);
    /// Valida el movimiento del alfil.
    bool checkMoveAlfil(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board);
    /// Valida el movimiento del caballo.
    bool checkMoveCaballo(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board);
    /// Valida el movimiento del peón (incluye captura al paso).
    bool checkMovePeon(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, EnPassantState& peonPaso);

public:
    /// Constructor por defecto (pieza vacía).
    Piece();
    /// Crea una pieza en la casilla (x, y) con su textura, tipo y color.
    Piece(unsigned x, unsigned y, sf::Texture& texture, PieceType type, bool color);
    virtual ~Piece();

    /// Actualiza la pieza (sin lógica por ahora).
    void update();
    /// Dibuja la pieza.
    void render(sf::RenderTarget& target);

    /// Activa o desactiva la pieza (las capturadas quedan inactivas).
    void setActive(bool isActive);
    /// Cambia el color de la pieza.
    void setColor(bool color);
    /// Cambia el tipo de la pieza y ajusta su puntuación (usado al coronar).
    void setPieceType(PieceType type);
    /// Cambia la textura de la pieza.
    void setTexture(sf::Texture& texture);

    /// Devuelve el color (1 blanca, 0 negra).
    bool getColor() const;
    /// Devuelve el tipo de la pieza.
    PieceType getType() const;
    /// Devuelve la posición del sprite en píxeles.
    sf::Vector2i getPosition() const;
    /// Devuelve la posición en la rejilla del tablero.
    sf::Vector2i getGridPosition() const;
    /// Devuelve el valor en puntos de la pieza.
    int getPoints() const;
    /// Indica si la pieza sigue activa.
    bool isActive() const;

    /// Mueve la pieza a la casilla de rejilla (x, y).
    void move(int x, int y);

    /// Valida si el movimiento es legal para el tipo de esta pieza.
    bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, CastlingState& castling, EnPassantState& peonPaso);
};
