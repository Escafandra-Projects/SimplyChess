#pragma once

#include <string>
#include <vector>
#include <map>
#include "chess/MoveHistory.h"
#include "chess/Board.h"

class PGNManager {
public:
    /// Exporta la partida a un archivo PGN
    static bool exportToFile(const std::string& filepath,
                             const MoveHistory& history,
                             const std::string& white,
                             const std::string& black,
                             GameStatus status,
                             float baseTime = 0,
                             float increment = 0);
    
    /// Genera el contenido PGN como string
    static std::string generatePGN(const MoveHistory& history,
                                    const std::string& white,
                                    const std::string& black,
                                    GameStatus status,
                                    float baseTime = 0,
                                    float increment = 0);

private:
    /// Genera la cabecera PGN
    static std::string generateHeaders(const std::string& white,
                                        const std::string& black,
                                        const std::string& result,
                                        float baseTime,
                                        float increment);
    
    /// Determina el resultado
    static std::string determineResult(GameStatus status, bool lastTurn);
};
