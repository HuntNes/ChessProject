#ifndef CHESS3_ARCHER_H
#define CHESS3_ARCHER_H

#include "Piece.h"

class Archer : public Piece {
public:
    Archer(const std::string& color);
    bool isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard* board) const override;
    bool canAttack(int fromX, int fromY, int toX, int toY, ChessBoard* board) const;
    std::string getType() const override { return "Archer"; }
};

#endif //CHESS3_ARCHER_H 