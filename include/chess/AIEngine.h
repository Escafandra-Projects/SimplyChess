#pragma once

#include "chess/FastBoard.h"
#include "chess/MoveGen.h"
#include <vector>
#include <atomic>

class AIEngine {
public:
    // Obtener el mejor movimiento. 'maxDepth' controla la dificultad.
    // 'stopFlag' permite detener la búsqueda asíncronamente (multithreading).
    static FastMove getBestMove(const FastBoard& board, int maxDepth, std::atomic<bool>& stopFlag);
    
    // Evaluar la posición actual usando Minimax con poda Alfa-Beta
    static int evaluatePosition(const FastBoard& board, int depth, std::atomic<bool>& stopFlag);

    // Evaluar si la IA acepta una oferta de tablas (Escafandrin)
    static bool shouldAcceptDraw(const FastBoard& board);
    
private:
    static int evaluate(const FastBoard& board);
    static int evaluatePiece(uint8_t piece, int x, int y, bool isEndgame);
    
    static int minimax(FastBoard& board, int depth, int alpha, int beta, bool isMaximizing, std::atomic<bool>& stopFlag);
    
    // Función para ordenar movimientos y mejorar la poda Alfa-Beta
    static void orderMoves(std::vector<FastMove>& moves, const FastBoard& board);
};
