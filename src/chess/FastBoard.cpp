#include "chess/FastBoard.h"
#include <cstring>

FastBoard::FastBoard() {
    memset(grid, FAST_EMPTY, sizeof(grid));
    turnBlack = false;
    for (int i = 0; i < 4; ++i) castlingRights[i] = false;
    hasEnPassant = false;
    enPassantX = 0;
    enPassantY = 0;
}

void FastBoard::initFromBoardGrid(const BoardGrid& boardGrid, bool turn, const CastlingState& castling, const EnPassantState& peonPaso) {
    turnBlack = !turn; // turn==true es blancas en el juego original
    
    // Convertir el grid original al formato rápido
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const std::string& p = boardGrid[y][x];
            if (p == "+" || p == "-") {
                grid[y][x] = FAST_EMPTY;
            } else {
                uint8_t pieceType = FAST_EMPTY;
                uint8_t color = (p[1] == 'N') ? COLOR_BLACK : COLOR_WHITE;
                
                switch (p[0]) {
                    case 'P': pieceType = FAST_PAWN; break;
                    case 'C': pieceType = FAST_KNIGHT; break;
                    case 'A': pieceType = FAST_BISHOP; break;
                    case 'T': pieceType = FAST_ROOK; break;
                    case 'Q': pieceType = FAST_QUEEN; break;
                    case 'K': pieceType = FAST_KING; break;
                }
                
                grid[y][x] = pieceType | color;
            }
        }
    }
    
    // castling: 0=Blancas (bool_general), 1=Rey Blanco, 2=Reina Blanca, 3=Rey Negro, 4=Reina Negra
    // castling[0] is not directly useful here since we track specific rights
    castlingRights[0] = castling[1]; // Corto Blanco (Rey)
    castlingRights[1] = castling[2]; // Largo Blanco (Reina)
    castlingRights[2] = castling[3]; // Corto Negro (Rey)
    castlingRights[3] = castling[4]; // Largo Negro (Reina)
    
    // En Passant
    hasEnPassant = false;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            // peonPaso contiene si hay un peón vulnerable al paso.
            // turnBlack es el turno actual, entonces buscamos si el rival dejó un peón vulnerable
            // El array peonPaso tiene 2 booleanos por fila. El original es extraño (un array 8x2 en Y).
            // Revisemos cómo funciona exactamente en la base de código.
            // Para ser seguros, si encontramos un peón que se movió 2 casillas en el turno anterior, 
            // su Y será 3 (blancas) o 4 (negras). 
            if (peonPaso[y][0] || peonPaso[y][1]) {
                hasEnPassant = true;
                // Asumimos que x es la columna (aunque peonPaso no almacena X tan fácil, pero sí Y).
                // Necesitaremos buscar qué peón causó esto. Por ahora lo dejamos desactivado o lo buscamos:
            }
        }
    }
    
    // Buscar la posición exacta del peón al paso si existe
    if (hasEnPassant) {
        uint8_t targetColor = turnBlack ? COLOR_WHITE : COLOR_BLACK;
        uint8_t targetY = turnBlack ? 3 : 4; // Blancas están en 3 (movidas desde 1), Negras en 4 (movidas desde 6)
        for (int x = 0; x < 8; ++x) {
            if (grid[targetY][x] == (FAST_PAWN | targetColor)) {
                if (peonPaso[targetY][targetColor == COLOR_WHITE ? 1 : 0]) { // Color en el juego 1=blancas, 0=negras
                    enPassantX = x;
                    enPassantY = targetY;
                    break;
                }
            }
        }
    }
}

void FastBoard::applyMove(const FastMove& move) {
    // Aplicar el movimiento en el grid
    grid[move.toY][move.toX] = move.movedPiece;
    grid[move.fromY][move.fromX] = FAST_EMPTY;
    
    if (move.isPromotion) {
        uint8_t color = move.movedPiece & COLOR_BLACK;
        grid[move.toY][move.toX] = move.promotionType | color;
    }
    
    if (move.isEnPassant) {
        // Eliminar el peón capturado
        grid[enPassantY][move.toX] = FAST_EMPTY;
    }
    
    if (move.isCastleShort) {
        // Mover torre (H a F)
        uint8_t rookColor = move.movedPiece & COLOR_BLACK;
        grid[move.toY][5] = FAST_ROOK | rookColor;
        grid[move.fromY][7] = FAST_EMPTY;
    }
    else if (move.isCastleLong) {
        // Mover torre (A a D)
        uint8_t rookColor = move.movedPiece & COLOR_BLACK;
        grid[move.toY][3] = FAST_ROOK | rookColor;
        grid[move.fromY][0] = FAST_EMPTY;
    }
    
    // Actualizar En Passant
    hasEnPassant = false;
    if (getFastType(move.movedPiece) == FAST_PAWN && abs(move.toY - move.fromY) == 2) {
        hasEnPassant = true;
        enPassantX = move.toX;
        enPassantY = move.toY;
    }
    
    // Actualizar Castling Rights (si mueve el rey o una torre, se pierden derechos)
    uint8_t type = getFastType(move.movedPiece);
    if (type == FAST_KING) {
        if (!turnBlack) { castlingRights[0] = false; castlingRights[1] = false; }
        else { castlingRights[2] = false; castlingRights[3] = false; }
    } else if (type == FAST_ROOK) {
        if (!turnBlack) {
            if (move.fromX == 7) castlingRights[0] = false; // Torre H blanca
            if (move.fromX == 0) castlingRights[1] = false; // Torre A blanca
        } else {
            if (move.fromX == 7) castlingRights[2] = false; // Torre H negra
            if (move.fromX == 0) castlingRights[3] = false; // Torre A negra
        }
    }
    // Si capturan una torre original, también se pierde el enroque
    if (move.toY == 0 && move.toX == 7) castlingRights[2] = false; // Torre H negra
    if (move.toY == 0 && move.toX == 0) castlingRights[3] = false; // Torre A negra
    if (move.toY == 7 && move.toX == 7) castlingRights[0] = false; // Torre H blanca
    if (move.toY == 7 && move.toX == 0) castlingRights[1] = false; // Torre A blanca
    
    // Cambiar turno
    turnBlack = !turnBlack;
}

void FastBoard::undoMove(const FastMove& move, const bool oldCastling[4], bool oldHasEP, uint8_t oldEpX, uint8_t oldEpY) {
    // Revertir el turno
    turnBlack = !turnBlack;
    
    // Restaurar el grid
    grid[move.fromY][move.fromX] = move.movedPiece;
    grid[move.toY][move.toX] = move.capturedPiece; // Puede ser FAST_EMPTY
    
    if (move.isEnPassant) {
        // En passant captura en una casilla distinta a 'toY', 'toX'
        grid[move.toY][move.toX] = FAST_EMPTY;
        uint8_t pawnColor = turnBlack ? COLOR_WHITE : COLOR_BLACK;
        grid[oldEpY][move.toX] = FAST_PAWN | pawnColor;
    }
    
    if (move.isCastleShort) {
        uint8_t rookColor = move.movedPiece & COLOR_BLACK;
        grid[move.fromY][7] = FAST_ROOK | rookColor;
        grid[move.fromY][5] = FAST_EMPTY;
    }
    else if (move.isCastleLong) {
        uint8_t rookColor = move.movedPiece & COLOR_BLACK;
        grid[move.fromY][0] = FAST_ROOK | rookColor;
        grid[move.fromY][3] = FAST_EMPTY;
    }
    
    // Restaurar estado
    for (int i = 0; i < 4; ++i) castlingRights[i] = oldCastling[i];
    hasEnPassant = oldHasEP;
    enPassantX = oldEpX;
    enPassantY = oldEpY;
}
