#include "chess/PGNManager.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

std::string PGNManager::determineResult(GameStatus status, bool lastTurn) {
    if (status == GameStatus::CHECKMATE || status == GameStatus::TIMEOUT) {
        return lastTurn ? "0-1" : "1-0"; // If it was white's turn (true), black wins, so 0-1. 
    }
    if (status == GameStatus::STALEMATE || status == GameStatus::FIFTY_MOVE_RULE || status == GameStatus::REPETITION) {
        return "1/2-1/2";
    }
    return "*";
}

std::string PGNManager::generateHeaders(const std::string& white,
                                        const std::string& black,
                                        const std::string& result,
                                        float baseTime,
                                        float increment) {
    std::ostringstream oss;
    
    // Date
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    char dateStr[20];
    std::strftime(dateStr, sizeof(dateStr), "%Y.%m.%d", now);
    
    oss << "[Event \"Simply Chess Game\"]\n";
    oss << "[Site \"Local\"]\n";
    oss << "[Date \"" << dateStr << "\"]\n";
    oss << "[Round \"1\"]\n";
    oss << "[White \"" << white << "\"]\n";
    oss << "[Black \"" << black << "\"]\n";
    oss << "[Result \"" << result << "\"]\n";
    
    if (baseTime > 0) {
        oss << "[TimeControl \"" << static_cast<int>(baseTime) << "+" << static_cast<int>(increment) << "\"]\n";
    } else {
        oss << "[TimeControl \"-\"]\n";
    }
    
    oss << "\n";
    return oss.str();
}

std::string PGNManager::generatePGN(const MoveHistory& history,
                                    const std::string& white,
                                    const std::string& black,
                                    GameStatus status,
                                    float baseTime,
                                    float increment) {
    // Determine result based on status. We need to know who just moved.
    // If it's checkmate and it's currently Black's turn, then White delivered checkmate, so 1-0.
    // Actually, in GameState `turn` holds the NEXT player's turn.
    // So if status is checkmate and turn is false (Black), White won: 1-0.
    bool currentTurn = (history.size() % 2 == 0); // If 0 moves, true (White).
    std::string result = determineResult(status, currentTurn);
    
    std::string pgn = generateHeaders(white, black, result, baseTime, increment);
    
    pgn += history.toPGNMovetext();
    
    // Add result at the end of the movetext
    if (!pgn.empty() && pgn.back() != ' ' && pgn.back() != '\n') {
        pgn += " ";
    }
    pgn += result + "\n";
    
    return pgn;
}

bool PGNManager::exportToFile(const std::string& filepath,
                              const MoveHistory& history,
                              const std::string& white,
                              const std::string& black,
                              GameStatus status,
                              float baseTime,
                              float increment) {
    std::string pgnContent = generatePGN(history, white, black, status, baseTime, increment);
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) return false;
    ofs << pgnContent;
    ofs.close();
    return true;
}
