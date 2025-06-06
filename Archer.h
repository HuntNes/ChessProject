#ifndef CHESS3_ARCHER_H
#define CHESS3_ARCHER_H

#include "Piece.h"
#include "ChessBoard.h"

class Archer : public Piece {
public:
    Archer(const std::string& color);
    bool isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard* board) const;
    bool canAttack(int fromX, int fromY, int toX, int toY, ChessBoard* board) const;
    std::string getType() const { return "Archer"; }
};

#endif //CHESS3_ARCHER_H 