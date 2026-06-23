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

// --- Orientación de la vista del tablero ---
// Si la vista está volteada (perspectiva de negras), la fila/columna 0 se dibuja
// en la esquina opuesta. Afecta solo al render y a la conversión de coordenadas;
// la lógica del tablero (BoardGrid) es independiente de la orientación.
void setBoardFlipped(bool flipped);
bool isBoardFlipped();

/// Convierte una columna (gridPos.y) en la coordenada X en píxeles de la casilla.
float colToPixelX(int col);
/// Convierte una fila (gridPos.x) en la coordenada Y en píxeles de la casilla.
float rowToPixelY(int row);
/// Convierte una coordenada X en píxeles en índice de columna.
int pixelXToCol(int px);
/// Convierte una coordenada Y en píxeles en índice de fila.
int pixelYToRow(int py);

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

    // Animación
    sf::Vector2f targetPos;
    bool isAnimating;
    static constexpr float ANIMATION_SPEED = 15.0f; // Constante fija de velocidad (15x distancia por segundo)

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

    /// Actualiza la animación de la pieza.
    void update(float dt);
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
    /// Indica si la pieza se está animando.
    bool getIsAnimating() const;

    /// Mueve la pieza a la casilla de rejilla (x, y).
    void move(int x, int y);

    /// Mueve la pieza a una posición de grid inmediatamente (sin animación)
    void setGridPositionImmediate(unsigned x, unsigned y);


    /// Coloca el sprite directamente en píxeles (para arrastrarlo, sin animación
    /// ni cambiar la casilla lógica).
    void setRenderPosition(float x, float y);
    /// Devuelve el sprite a su casilla de rejilla actual, sin animación
    /// (para cancelar un arrastre).
    void snapToGrid();

    /// Valida si el movimiento es legal para el tipo de esta pieza.
    bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos, BoardGrid& board, CastlingState& castling, EnPassantState& peonPaso);
};
