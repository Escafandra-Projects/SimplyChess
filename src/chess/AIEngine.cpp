#include "chess/AIEngine.h"
#include <algorithm>
#include <iostream>
#include <cstdlib> // For std::rand()

// Tablas de puntuación para animar a las piezas a ir al centro (Perspectiva Blancas)
const int pawnEvalWhite[8][8] = {
    {0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {5,  5, 10, 25, 25, 10,  5,  5},
    {0,  0,  0, 20, 20,  0,  0,  0},
    {5, -5,-10,  0,  0,-10, -5,  5},
    {5, 10, 10,-20,-20, 10, 10,  5},
    {0,  0,  0,  0,  0,  0,  0,  0}
};

const int knightEval[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int bishopEvalWhite[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int rookEvalWhite[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

const int queenEval[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int kingEvalWhite[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};


FastMove AIEngine::getBestMove(const FastBoard& board, int maxDepth, std::atomic<bool>& stopFlag) {
    FastMove bestMove; // FAST_EMPTY
    
    std::vector<FastMove> moves = MoveGen::generateLegalMoves(board);
    if (moves.empty()) return bestMove;
    
    orderMoves(moves, board);
    FastBoard searchBoard = board;

    std::vector<std::pair<FastMove, int>> evaluatedMoves;
    
    // Solo usamos poda Alfa-Beta en la raíz en la dificultad más alta (maxDepth >= 4)
    // para obtener un rendimiento óptimo de búsqueda. En dificultades bajas/medias
    // necesitamos evaluar todos los movimientos exactos para aplicar la lógica de dificultad.
    bool useRootPruning = (maxDepth >= 4);
    
    int alpha = -500000;
    int beta = 500000;
    int bestValue = board.turnBlack ? 500000 : -500000;

    for (const FastMove& move : moves) {
        if (stopFlag) break;

        bool oldCastling[4];
        for (int i=0; i<4; ++i) oldCastling[i] = searchBoard.castlingRights[i];
        bool oldHasEp = searchBoard.hasEnPassant;
        uint8_t oldEpX = searchBoard.enPassantX;
        uint8_t oldEpY = searchBoard.enPassantY;

        searchBoard.applyMove(move);
        bool isMaximizing = !searchBoard.turnBlack; 
        
        int boardValue = minimax(searchBoard, maxDepth - 1, alpha, beta, isMaximizing, stopFlag);
        
        searchBoard.undoMove(move, oldCastling, oldHasEp, oldEpX, oldEpY);

        evaluatedMoves.push_back({move, boardValue});

        if (useRootPruning) {
            if (board.turnBlack) { // Negras minimizan
                if (boardValue < bestValue) {
                    bestValue = boardValue;
                    bestMove = move;
                }
                beta = std::min(beta, boardValue);
            } else { // Blancas maximizan
                if (boardValue > bestValue) {
                    bestValue = boardValue;
                    bestMove = move;
                }
                alpha = std::max(alpha, boardValue);
            }
        }
    }

    if (evaluatedMoves.empty()) {
        return bestMove;
    }

    if (!useRootPruning) {
        // Ordenar movimientos según su valor
        if (!board.turnBlack) { // Juegan blancas, quieren el mayor valor posible
            std::sort(evaluatedMoves.begin(), evaluatedMoves.end(), [](const auto& a, const auto& b) {
                return a.second > b.second;
            });
        } else { // Juegan negras, quieren el menor valor posible
            std::sort(evaluatedMoves.begin(), evaluatedMoves.end(), [](const auto& a, const auto& b) {
                return a.second < b.second;
            });
        }

        // Lógica de dificultad ("emoción")
        int chosenIndex = 0;
        
        if (maxDepth == 2) { // Easy / Baja
            // Elegir aleatoriamente entre los 3 mejores movimientos si la diferencia de puntaje no es catastrófica (ej. regalar la dama)
            int maxIdx = std::min(3, (int)evaluatedMoves.size());
            int validOptions = 1;
            for (int i = 1; i < maxIdx; ++i) {
                if (std::abs(evaluatedMoves[i].second - evaluatedMoves[0].second) <= 300) {
                    validOptions++;
                }
            }
            chosenIndex = std::rand() % validOptions;
        } else if (maxDepth == 3) { // Normal
            // 20% de probabilidad de elegir el 2do mejor movimiento (si la diferencia no es catastrófica)
            if (evaluatedMoves.size() > 1 && (std::rand() % 100) < 20) {
                if (std::abs(evaluatedMoves[1].second - evaluatedMoves[0].second) <= 150) {
                    chosenIndex = 1;
                }
            }
        }

        bestMove = evaluatedMoves[chosenIndex].first;
    } else {
        // Si usamos poda en la raíz y por alguna razón (ej. stopFlag) bestMove no quedó asignada,
        // tomamos el mejor movimiento de los evaluados.
        if (bestMove.movedPiece == 0 && !evaluatedMoves.empty()) {
            int bestIdx = 0;
            for (size_t i = 1; i < evaluatedMoves.size(); ++i) {
                if (board.turnBlack) {
                    if (evaluatedMoves[i].second < evaluatedMoves[bestIdx].second) bestIdx = i;
                } else {
                    if (evaluatedMoves[i].second > evaluatedMoves[bestIdx].second) bestIdx = i;
                }
            }
            bestMove = evaluatedMoves[bestIdx].first;
        }
    }
    
    if (bestMove.movedPiece == 0 && !moves.empty()) {
        bestMove = moves[0];
    }
    
    return bestMove;
}

int AIEngine::minimax(FastBoard& board, int depth, int alpha, int beta, bool isMaximizing, std::atomic<bool>& stopFlag) {
    if (stopFlag) return 0;
    if (depth == 0) {
        return evaluate(board);
    }

    std::vector<FastMove> moves = MoveGen::generateLegalMoves(board);

    if (moves.empty()) {
        if (MoveGen::isInCheck(board)) {
            // Mate. La distancia penaliza.
            return isMaximizing ? -300000 + (100 - depth) : 300000 - (100 - depth);
        }
        return 0; // Ahogado
    }

    orderMoves(moves, board);

    if (isMaximizing) { // Juegan Blancas
        int maxEval = -500000;
        for (const FastMove& move : moves) {
            bool oldCastling[4];
            for (int i=0; i<4; ++i) oldCastling[i] = board.castlingRights[i];
            bool oldHasEp = board.hasEnPassant;
            uint8_t oldEpX = board.enPassantX;
            uint8_t oldEpY = board.enPassantY;

            board.applyMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false, stopFlag);
            board.undoMove(move, oldCastling, oldHasEp, oldEpX, oldEpY);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Poda Beta
        }
        return maxEval;
    } else { // Juegan Negras
        int minEval = 500000;
        for (const FastMove& move : moves) {
            bool oldCastling[4];
            for (int i=0; i<4; ++i) oldCastling[i] = board.castlingRights[i];
            bool oldHasEp = board.hasEnPassant;
            uint8_t oldEpX = board.enPassantX;
            uint8_t oldEpY = board.enPassantY;

            board.applyMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true, stopFlag);
            board.undoMove(move, oldCastling, oldHasEp, oldEpX, oldEpY);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Poda Alfa
        }
        return minEval;
    }
}

int AIEngine::evaluate(const FastBoard& board) {
    int score = 0;
    
    // Contamos material para saber si estamos en el "endgame" (simplificación)
    int totalMaterial = 0;
    
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            uint8_t piece = board.grid[y][x];
            if (piece != FAST_EMPTY) {
                int pieceValue = evaluatePiece(piece, x, y, false); // Pendiente optimizar isEndgame
                if ((piece & COLOR_BLACK) == COLOR_WHITE) {
                    score += pieceValue;
                    totalMaterial += pieceValue;
                } else {
                    score -= pieceValue;
                    totalMaterial += pieceValue;
                }
            }
        }
    }
    
    return score;
}

int AIEngine::evaluatePiece(uint8_t piece, int x, int y, bool isEndgame) {
    uint8_t type = getFastType(piece);
    bool isBlack = (piece & COLOR_BLACK) == COLOR_BLACK;
    
    // Para las negras, la tabla se invierte verticalmente
    int tableY = isBlack ? (7 - y) : y;
    
    int value = 0;
    switch (type) {
        case FAST_PAWN:   value = 100 + pawnEvalWhite[tableY][x]; break;
        case FAST_KNIGHT: value = 320 + knightEval[tableY][x]; break;
        case FAST_BISHOP: value = 330 + bishopEvalWhite[tableY][x]; break;
        case FAST_ROOK:   value = 500 + rookEvalWhite[tableY][x]; break;
        case FAST_QUEEN:  value = 900 + queenEval[tableY][x]; break;
        case FAST_KING:   value = 20000 + kingEvalWhite[tableY][x]; break;
    }
    return value;
}

void AIEngine::orderMoves(std::vector<FastMove>& moves, const FastBoard& board) {
    // Ordenar para intentar revisar primero las mejores jugadas (aumenta muchísimo la poda Alfa-Beta)
    // Criterio: Capturas (Material ganado - Material atacante) > Promociones > Resto
    std::sort(moves.begin(), moves.end(), [](const FastMove& a, const FastMove& b) {
        int scoreA = 0;
        int scoreB = 0;
        
        if (a.capturedPiece != FAST_EMPTY) {
            // MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
            scoreA = 10 * getFastType(a.capturedPiece) - getFastType(a.movedPiece);
        }
        if (a.isPromotion) scoreA += 900;
        
        if (b.capturedPiece != FAST_EMPTY) {
            scoreB = 10 * getFastType(b.capturedPiece) - getFastType(b.movedPiece);
        }
        if (b.isPromotion) scoreB += 900;
        
        return scoreA > scoreB;
    });
}

bool AIEngine::shouldAcceptDraw(const FastBoard& board) {
    int score = evaluate(board);
    // Para la IA (negras), si score es muy positivo (ventaja blanca), significa que la IA esta perdiendo.
    // Aceptara tablas si la ventaja blanca es sustancial (p.ej., > 400 centipawns).
    return score > 400;
}
