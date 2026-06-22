#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "chess/Piece.h"

struct MoveRecord {
    // Datos del movimiento
    PieceType pieceType;       // Tipo de pieza que mueve
    bool color;                // Color de la pieza (true=blancas, false=negras)
    sf::Vector2i from;         // Casilla origen (grid coords)
    sf::Vector2i to;           // Casilla destino (grid coords)
    
    // Flags especiales
    bool isCapture;            // ¿Captura una pieza?
    bool isEnPassant;          // ¿Captura al paso?
    bool isCastleShort;        // ¿Enroque corto?
    bool isCastleLong;         // ¿Enroque largo?
    bool isPromotion;          // ¿Coronación?
    PieceType promotedTo;      // Pieza resultante de la coronación
    bool isCheck;              // ¿Da jaque?
    bool isCheckmate;          // ¿Da jaque mate?
    
    // Desambiguación (cuando dos piezas del mismo tipo pueden ir al mismo destino)
    bool needsFileDisambiguation;  // ¿Incluir columna de origen?
    bool needsRankDisambiguation;  // ¿Incluir fila de origen?
    
    // Pieza capturada (para undo)
    PieceType capturedType;
    int capturedPieceIndex;    // Índice en el array pieces[]
    
    /// Genera la notación algebraica estándar del movimiento (ej: "Nxf3+")
    std::string toSAN() const {
        if (isCastleShort) {
            std::string suffix = "";
            if (isCheckmate) suffix = "#";
            else if (isCheck) suffix = "+";
            return "O-O" + suffix;
        }
        if (isCastleLong) {
            std::string suffix = "";
            if (isCheckmate) suffix = "#";
            else if (isCheck) suffix = "+";
            return "O-O-O" + suffix;
        }

        std::string san = "";

        // 1. Prefijo de Pieza (universal en inglés)
        if (pieceType == PieceType::REY) {
            san += "K";
        } else if (pieceType == PieceType::REINA) {
            san += "Q";
        } else if (pieceType == PieceType::TORRE) {
            san += "R";
        } else if (pieceType == PieceType::ALFIL) {
            san += "B";
        } else if (pieceType == PieceType::CABALLO) {
            san += "N";
        } // Los peones no llevan prefijo de letra

        // 2. Desambiguación
        if (pieceType != PieceType::PEON) {
            if (needsFileDisambiguation) {
                san += (char)('a' + from.y);
            }
            if (needsRankDisambiguation) {
                san += (char)('8' - from.x);
            }
        } else {
            // Captura de peón requiere columna de origen
            if (isCapture) {
                san += (char)('a' + from.y);
            }
        }

        // 3. Indicador de captura
        if (isCapture) {
            san += "x";
        }

        // 4. Casilla destino
        san += gridToSquare(to);

        // 5. Coronación
        if (isPromotion) {
            san += "=";
            if (promotedTo == PieceType::REINA) {
                san += "Q";
            } else if (promotedTo == PieceType::TORRE) {
                san += "R";
            } else if (promotedTo == PieceType::ALFIL) {
                san += "B";
            } else if (promotedTo == PieceType::CABALLO) {
                san += "N";
            }
        }

        // 6. Jaque o Jaque Mate
        if (isCheckmate) {
            san += "#";
        } else if (isCheck) {
            san += "+";
        }

        return san;
    }
    
    /// Convierte coordenadas de grid a notación de casilla (ej: {4,3} → "e4")
    static std::string gridToSquare(sf::Vector2i gridPos) {
        char file = 'a' + gridPos.y;
        char rank = '8' - gridPos.x;
        return std::string{file, rank};
    }
};
