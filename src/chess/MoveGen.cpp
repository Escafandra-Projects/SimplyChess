#include "chess/MoveGen.h"

std::vector<FastMove> MoveGen::generatePseudoLegalMoves(const FastBoard& board) {
    std::vector<FastMove> moves;
    moves.reserve(60); // Reserva para evitar realojos
    
    uint8_t myColor = board.turnBlack ? COLOR_BLACK : COLOR_WHITE;
    
    for (uint8_t y = 0; y < 8; ++y) {
        for (uint8_t x = 0; x < 8; ++x) {
            uint8_t piece = board.grid[y][x];
            if (piece != FAST_EMPTY && (piece & COLOR_BLACK) == myColor) {
                uint8_t type = getFastType(piece);
                switch (type) {
                    case FAST_PAWN: generatePawnMoves(board, x, y, moves); break;
                    case FAST_KNIGHT: generateKnightMoves(board, x, y, moves); break;
                    case FAST_BISHOP: generateBishopMoves(board, x, y, moves); break;
                    case FAST_ROOK: generateRookMoves(board, x, y, moves); break;
                    case FAST_QUEEN: generateQueenMoves(board, x, y, moves); break;
                    case FAST_KING: generateKingMoves(board, x, y, moves); break;
                }
            }
        }
    }
    
    return moves;
}

std::vector<FastMove> MoveGen::generateLegalMoves(const FastBoard& board) {
    std::vector<FastMove> pseudoMoves = generatePseudoLegalMoves(board);
    std::vector<FastMove> legalMoves;
    legalMoves.reserve(pseudoMoves.size());
    
    for (const FastMove& move : pseudoMoves) {
        FastBoard tempBoard = board;
        tempBoard.applyMove(move);
        
        // Al aplicar, turnBlack cambió. Queremos saber si MI rey está en jaque
        // tempBoard.turnBlack es ahora el rival, así que volvemos a invertir mentalmente
        tempBoard.turnBlack = board.turnBlack; 
        
        if (!isInCheck(tempBoard)) {
            legalMoves.push_back(move);
        }
    }
    
    return legalMoves;
}

void MoveGen::addDirectionalMoves(const FastBoard& board, uint8_t x, uint8_t y, int dx, int dy, bool limitOne, std::vector<FastMove>& moves) {
    uint8_t myColor = board.turnBlack ? COLOR_BLACK : COLOR_WHITE;
    uint8_t piece = board.grid[y][x];
    
    int cx = x + dx;
    int cy = y + dy;
    
    while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
        uint8_t target = board.grid[cy][cx];
        
        if (target == FAST_EMPTY) {
            moves.emplace_back(x, y, cx, cy, piece, FAST_EMPTY);
            if (limitOne) break;
        } else {
            if ((target & COLOR_BLACK) != myColor) {
                // Captura enemiga
                moves.emplace_back(x, y, cx, cy, piece, target);
            }
            // Bloqueado por pieza propia o enemiga (ya añadida si era enemiga)
            break;
        }
        
        cx += dx;
        cy += dy;
    }
}

void MoveGen::generatePawnMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves) {
    int dir = board.turnBlack ? 1 : -1;
    uint8_t promotionY = board.turnBlack ? 7 : 0;
    uint8_t startY = board.turnBlack ? 1 : 6;
    uint8_t myColor = board.turnBlack ? COLOR_BLACK : COLOR_WHITE;
    uint8_t piece = board.grid[y][x];
    
    // Movimiento adelante
    int forwardY = y + dir;
    if (forwardY >= 0 && forwardY < 8 && board.grid[forwardY][x] == FAST_EMPTY) {
        if (forwardY == promotionY) {
            for (uint8_t p : {FAST_QUEEN, FAST_ROOK, FAST_BISHOP, FAST_KNIGHT}) {
                FastMove m(x, y, x, forwardY, piece, FAST_EMPTY);
                m.isPromotion = true;
                m.promotionType = p;
                moves.push_back(m);
            }
        } else {
            moves.emplace_back(x, y, x, forwardY, piece, FAST_EMPTY);
            // Salto doble
            if (y == startY) {
                int forward2Y = y + 2 * dir;
                if (board.grid[forward2Y][x] == FAST_EMPTY) {
                    moves.emplace_back(x, y, x, forward2Y, piece, FAST_EMPTY);
                }
            }
        }
    }
    
    // Capturas diagonales
    for (int dx : {-1, 1}) {
        int nx = x + dx;
        if (nx >= 0 && nx < 8 && forwardY >= 0 && forwardY < 8) {
            uint8_t target = board.grid[forwardY][nx];
            if (target != FAST_EMPTY && (target & COLOR_BLACK) != myColor) {
                if (forwardY == promotionY) {
                    for (uint8_t p : {FAST_QUEEN, FAST_ROOK, FAST_BISHOP, FAST_KNIGHT}) {
                        FastMove m(x, y, nx, forwardY, piece, target);
                        m.isPromotion = true;
                        m.promotionType = p;
                        moves.push_back(m);
                    }
                } else {
                    moves.emplace_back(x, y, nx, forwardY, piece, target);
                }
            }
            // En Passant
            else if (board.hasEnPassant && board.enPassantX == nx && board.enPassantY == forwardY) {
                FastMove m(x, y, nx, forwardY, piece, FAST_EMPTY); // Se maneja specialmente
                m.isEnPassant = true;
                m.capturedPiece = FAST_PAWN | (board.turnBlack ? COLOR_WHITE : COLOR_BLACK);
                moves.push_back(m);
            }
        }
    }
}

void MoveGen::generateKnightMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves) {
    int offsets[8][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
    for (auto& off : offsets) {
        addDirectionalMoves(board, x, y, off[0], off[1], true, moves);
    }
}

void MoveGen::generateBishopMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves) {
    addDirectionalMoves(board, x, y, -1, -1, false, moves);
    addDirectionalMoves(board, x, y, -1,  1, false, moves);
    addDirectionalMoves(board, x, y,  1, -1, false, moves);
    addDirectionalMoves(board, x, y,  1,  1, false, moves);
}

void MoveGen::generateRookMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves) {
    addDirectionalMoves(board, x, y, -1,  0, false, moves);
    addDirectionalMoves(board, x, y,  1,  0, false, moves);
    addDirectionalMoves(board, x, y,  0, -1, false, moves);
    addDirectionalMoves(board, x, y,  0,  1, false, moves);
}

void MoveGen::generateQueenMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves) {
    generateBishopMoves(board, x, y, moves);
    generateRookMoves(board, x, y, moves);
}

void MoveGen::generateKingMoves(const FastBoard& board, uint8_t x, uint8_t y, std::vector<FastMove>& moves) {
    int offsets[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    for (auto& off : offsets) {
        addDirectionalMoves(board, x, y, off[0], off[1], true, moves);
    }
    
    // Enroques
    uint8_t piece = board.grid[y][x];
    if (board.turnBlack) {
        if (board.castlingRights[2] && board.grid[0][5] == FAST_EMPTY && board.grid[0][6] == FAST_EMPTY) {
            if (!isSquareAttacked(board, 4, 0, false) && !isSquareAttacked(board, 5, 0, false) && !isSquareAttacked(board, 6, 0, false)) {
                FastMove m(x, y, 6, 0, piece, FAST_EMPTY);
                m.isCastleShort = true;
                moves.push_back(m);
            }
        }
        if (board.castlingRights[3] && board.grid[0][1] == FAST_EMPTY && board.grid[0][2] == FAST_EMPTY && board.grid[0][3] == FAST_EMPTY) {
            if (!isSquareAttacked(board, 4, 0, false) && !isSquareAttacked(board, 3, 0, false) && !isSquareAttacked(board, 2, 0, false)) {
                FastMove m(x, y, 2, 0, piece, FAST_EMPTY);
                m.isCastleLong = true;
                moves.push_back(m);
            }
        }
    } else {
        if (board.castlingRights[0] && board.grid[7][5] == FAST_EMPTY && board.grid[7][6] == FAST_EMPTY) {
            if (!isSquareAttacked(board, 4, 7, true) && !isSquareAttacked(board, 5, 7, true) && !isSquareAttacked(board, 6, 7, true)) {
                FastMove m(x, y, 6, 7, piece, FAST_EMPTY);
                m.isCastleShort = true;
                moves.push_back(m);
            }
        }
        if (board.castlingRights[1] && board.grid[7][1] == FAST_EMPTY && board.grid[7][2] == FAST_EMPTY && board.grid[7][3] == FAST_EMPTY) {
            if (!isSquareAttacked(board, 4, 7, true) && !isSquareAttacked(board, 3, 7, true) && !isSquareAttacked(board, 2, 7, true)) {
                FastMove m(x, y, 2, 7, piece, FAST_EMPTY);
                m.isCastleLong = true;
                moves.push_back(m);
            }
        }
    }
}

bool MoveGen::isSquareAttacked(const FastBoard& board, uint8_t x, uint8_t y, bool attackerIsBlack) {
    uint8_t attackerColor = attackerIsBlack ? COLOR_BLACK : COLOR_WHITE;
    
    // Peones
    int pawnDir = attackerIsBlack ? -1 : 1; // Si atacan las negras, vienen desde arriba (-1 en y hacia abajo del rey, pero relativo al rey es -1 en y)
    int attackY = y + pawnDir; // Casilla de la que vendría el peón atacante
    if (attackY >= 0 && attackY < 8) {
        if (x > 0 && board.grid[attackY][x-1] == (FAST_PAWN | attackerColor)) return true;
        if (x < 7 && board.grid[attackY][x+1] == (FAST_PAWN | attackerColor)) return true;
    }
    
    // Caballos
    int knightOffsets[8][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
    for (auto& off : knightOffsets) {
        int cx = x + off[0], cy = y + off[1];
        if (cx >= 0 && cx < 8 && cy >= 0 && cy < 8 && board.grid[cy][cx] == (FAST_KNIGHT | attackerColor)) return true;
    }
    
    // Rey
    int kingOffsets[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    for (auto& off : kingOffsets) {
        int cx = x + off[0], cy = y + off[1];
        if (cx >= 0 && cx < 8 && cy >= 0 && cy < 8 && board.grid[cy][cx] == (FAST_KING | attackerColor)) return true;
    }
    
    // Líneas (Torre y Reina)
    int lineOffsets[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    for (auto& off : lineOffsets) {
        int cx = x + off[0], cy = y + off[1];
        while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
            uint8_t p = board.grid[cy][cx];
            if (p != FAST_EMPTY) {
                if ((p & COLOR_BLACK) == attackerColor && (getFastType(p) == FAST_ROOK || getFastType(p) == FAST_QUEEN)) return true;
                break;
            }
            cx += off[0]; cy += off[1];
        }
    }
    
    // Diagonales (Alfil y Reina)
    int diagOffsets[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
    for (auto& off : diagOffsets) {
        int cx = x + off[0], cy = y + off[1];
        while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
            uint8_t p = board.grid[cy][cx];
            if (p != FAST_EMPTY) {
                if ((p & COLOR_BLACK) == attackerColor && (getFastType(p) == FAST_BISHOP || getFastType(p) == FAST_QUEEN)) return true;
                break;
            }
            cx += off[0]; cy += off[1];
        }
    }
    
    return false;
}

bool MoveGen::isInCheck(const FastBoard& board) {
    uint8_t myColor = board.turnBlack ? COLOR_BLACK : COLOR_WHITE;
    uint8_t kingPiece = FAST_KING | myColor;
    
    // Encontrar al rey
    for (uint8_t y = 0; y < 8; ++y) {
        for (uint8_t x = 0; x < 8; ++x) {
            if (board.grid[y][x] == kingPiece) {
                return isSquareAttacked(board, x, y, !board.turnBlack);
            }
        }
    }
    return false;
}
