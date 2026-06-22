#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <map>
#include <string>

#include "chess/Piece.h"

// Forward declare enum class from Board.h
enum class GameStatus;

/// Snapshot del estado de una pieza individual
struct PieceSnapshot {
    sf::Vector2i gridPos;
    bool active;
    PieceType type;
    bool color;
};

/// Snapshot completo del estado de la partida
struct GameSnapshot {
    // Estado del tablero
    BoardGrid board;                             // Grid 8x8
    std::array<PieceSnapshot, 32> pieces;        // Estado de las 32 piezas
    CastlingState castling;                      // Derechos de enroque
    EnPassantState peonPaso;                     // Elegibilidad de captura al paso
    std::array<bool, 2> jaque;                   // Estado de jaque
    GameStatus status;                           // Estado de la partida
    int halfMoveClock;                           // Contador de 50 movimientos
    std::map<std::string, int> positionHistory;  // Historial de posiciones
    MoveHistory moveHistory;                     // Historial de movimientos
    
    // Estado de la partida (en GameState).
    // captureSnapshot() no rellena estos campos (son propiedad de GameState),
    // así que los inicializamos para evitar lecturas de memoria indeterminada.
    bool turn = true;
    int points1 = 0;
    int points2 = 0;
    float timeWhite = 0.f;
    float timeBlack = 0.f;
    
    // Último movimiento (visual)
    bool hasLastMove;
    sf::Vector2f lastMoveStartPos;
    sf::Vector2f lastMoveEndPos;
};
