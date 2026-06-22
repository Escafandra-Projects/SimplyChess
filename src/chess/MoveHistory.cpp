#include "chess/MoveHistory.h"
#include <stdexcept>

void MoveHistory::addMove(const MoveRecord& move) {
    moves.push_back(move);
}

void MoveHistory::removeLast(int n) {
    for (int i = 0; i < n && !moves.empty(); ++i) {
        moves.pop_back();
    }
}

const MoveRecord& MoveHistory::getLastMove() const {
    if (moves.empty()) {
        throw std::runtime_error("MoveHistory is empty");
    }
    return moves.back();
}

MoveRecord& MoveHistory::getLastMoveRef() {
    if (moves.empty()) {
        throw std::runtime_error("MoveHistory is empty");
    }
    return moves.back();
}

const MoveRecord& MoveHistory::getMove(int index) const {
    return moves.at(index);
}

int MoveHistory::size() const {
    return static_cast<int>(moves.size());
}

void MoveHistory::clear() {
    moves.clear();
}

const std::vector<MoveRecord>& MoveHistory::getMoves() const {
    return moves;
}

std::string MoveHistory::toFormattedText() const {
    std::string text = "";
    for (size_t i = 0; i < moves.size(); i += 2) {
        int moveNum = (i / 2) + 1;
        text += std::to_string(moveNum) + ". " + moves[i].toSAN();
        if (i + 1 < moves.size()) {
            text += "  " + moves[i + 1].toSAN() + "\n";
        } else {
            text += "\n";
        }
    }
    return text;
}

std::string MoveHistory::toPGNMovetext() const {
    std::string text = "";
    for (size_t i = 0; i < moves.size(); ++i) {
        if (i % 2 == 0) {
            int moveNum = (i / 2) + 1;
            text += std::to_string(moveNum) + ". ";
        }
        text += moves[i].toSAN() + " ";
    }
    if (!text.empty() && text.back() == ' ') {
        text.pop_back();
    }
    return text;
}
