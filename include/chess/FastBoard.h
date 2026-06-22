#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "chess/Piece.h"

// Codificación de piezas para máxima velocidad en la IA (1 byte por casilla)
// Formato: 
// Bit 0-2: Tipo de pieza (0=Vacío, 1=Peón, 2=Caballo, 3=Alfil, 4=Torre, 5=Reina, 6=Rey)
// Bit 3: Color (0=Blanco, 1=Negro)
// 0x00 representa una casilla vacía.

constexpr uint8_t FAST_EMPTY = 0;

constexpr uint8_t FAST_PAWN = 1;
constexpr uint8_t FAST_KNIGHT = 2;
constexpr uint8_t FAST_BISHOP = 3;
constexpr uint8_t FAST_ROOK = 4;
constexpr uint8_t FAST_QUEEN = 5;
constexpr uint8_t FAST_KING = 6;

constexpr uint8_t COLOR_WHITE = 0x00;
constexpr uint8_t COLOR_BLACK = 0x08;

inline uint8_t getFastType(uint8_t piece) {
    return piece & 0x07;
}

inline bool isFastColor(uint8_t piece, bool colorBlack) {
    return piece != FAST_EMPTY && ((piece & COLOR_BLACK) == (colorBlack ? COLOR_BLACK : COLOR_WHITE));
}

// Representación de un movimiento súper ligero para el árbol de búsqueda
struct FastMove {
    uint8_t fromX;
    uint8_t fromY;
    uint8_t toX;
    uint8_t toY;
    
    uint8_t movedPiece;
    uint8_t capturedPiece;
    
    // Flags especiales
    bool isEnPassant : 1;
    bool isCastleShort : 1;
    bool isCastleLong : 1;
    bool isPromotion : 1;
    uint8_t promotionType : 4; // Tipo de pieza a coronar
    
    FastMove() : fromX(0), fromY(0), toX(0), toY(0), movedPiece(0), capturedPiece(0),
                 isEnPassant(false), isCastleShort(false), isCastleLong(false), isPromotion(false), promotionType(0) {}
                 
    FastMove(uint8_t fx, uint8_t fy, uint8_t tx, uint8_t ty, uint8_t moved, uint8_t captured = 0)
        : fromX(fx), fromY(fy), toX(tx), toY(ty), movedPiece(moved), capturedPiece(captured),
          isEnPassant(false), isCastleShort(false), isCastleLong(false), isPromotion(false), promotionType(0) {}
};

// Representación completa pero ligera del tablero para Minimax
struct FastBoard {
    uint8_t grid[8][8];
    
    // Estado adicional
    bool turnBlack; // true = Negras, false = Blancas
    
    // Enroques: indexado por color y lado (0=Corto Blanco, 1=Largo Blanco, 2=Corto Negro, 3=Largo Negro)
    bool castlingRights[4]; 
    
    // Peón al paso (si el movimiento anterior fue un salto doble de peón)
    bool hasEnPassant;
    uint8_t enPassantX;
    uint8_t enPassantY; // Coordenada del peón que acaba de saltar
    
    FastBoard();
    
    // Inicializar desde un string grid "PB", "TN", etc.
    void initFromBoardGrid(const BoardGrid& boardGrid, bool turn, const CastlingState& castling, const EnPassantState& peonPaso);
    
    // Aplicar un movimiento y modificar el tablero in-place (para no copiar tanto)
    void applyMove(const FastMove& move);
    
    // Deshacer un movimiento in-place
    void undoMove(const FastMove& move, const bool oldCastling[4], bool oldHasEP, uint8_t oldEpX, uint8_t oldEpY);
};
