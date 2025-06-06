#pragma once
#include "ChessBoard.h"
#include "Piece.h"
#include "Portal.h"
#include <vector>

class MoveValidator {
private:
    ChessBoard* board;

public:
    MoveValidator(ChessBoard* board);
    bool isValidLinearMove(int fromX, int fromY, int toX, int toY) const;
    bool isPathClear(int fromX, int fromY, int toX, int toY) const;
    bool validateMove(Piece* piece, int fromX, int fromY, int toX, int toY, const std::vector<Portal>& portals) const;
private:
    bool bfsWithPortals(Piece* piece, int fromX, int fromY, int toX, int toY, const std::vector<Portal>& portals) const;
};