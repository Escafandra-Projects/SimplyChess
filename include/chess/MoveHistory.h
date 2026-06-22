#pragma once

#include <vector>
#include <string>
#include "chess/MoveRecord.h"

class MoveHistory {
private:
    std::vector<MoveRecord> moves;
    
public:
    /// Añade un movimiento al historial
    void addMove(const MoveRecord& move);
    
    /// Elimina los últimos n movimientos (para undo)
    void removeLast(int n = 1);
    
    /// Obtiene el último movimiento
    const MoveRecord& getLastMove() const;
    
    /// Obtiene el último movimiento como referencia modificable (para coronación deferred)
    MoveRecord& getLastMoveRef();
    
    /// Obtiene el movimiento en el índice dado
    const MoveRecord& getMove(int index) const;
    
    /// Número total de movimientos
    int size() const;
    
    /// Limpia el historial
    void clear();
    
    /// Acceso a todos los movimientos
    const std::vector<MoveRecord>& getMoves() const;
    
    /// Genera el texto formateado del historial (para el panel)
    /// Formato: "1. e4  e5\n2. Nf3  Nc6\n..."
    std::string toFormattedText() const;
    
    /// Genera el movetext para PGN (una sola línea con wrapping)
    std::string toPGNMovetext() const;
};
