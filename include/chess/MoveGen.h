#pragma once

#include "chess/FastBoard.h"
#include <vector>

class MoveGen {
public:
    // Genera todos los movimientos legales para el color que tiene el turno
    static std::vector<FastMove> generateLegalMoves(const FastBoard& board);
    
    // Genera movimientos pseudo-legales (no comprueba si el rey queda en jaque)
    static std::vector<FastMove> generatePseudoLegalMoves(const FastBoard& board);
    
    // Comprueba si una casilla está siendo atacada por el bando enemigo
    static bool isSquareAttacked(const FastBoard& board, uint8_t x, uint8_t y, bool attackerIsBlack);
    
    // Evalúa si el bando en turno está en jaque
    static bool isInCheck(const FastBoard& board);
    
private:
    static void generatePawnMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves);
    static void generateKnightMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves);
    static void generateBishopMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves);
    static void generateRookMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves);
    static void generateQueenMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves);
    static void generateKingMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves);
    
    static void addDirectionalMoves(const FastBoard& board, uint8_t x, uint8_t y, int dx, int dy, bool limitOne, std::vector<FastMove>& moves);
};
