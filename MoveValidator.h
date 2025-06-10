#pragma once
#include "ChessBoard.h"
#include "Piece.h"
#include "Portal.h"
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>

struct LastMove;

class MoveValidator {
private:
    ChessBoard* board;

public:
    MoveValidator(ChessBoard* board);
    bool isValidLinearMove(int fromX, int fromY, int toX, int toY) const;
    bool isPathClear(int fromX, int fromY, int toX, int toY) const;
    bool validateMove(Piece* piece, int fromX, int fromY, int toX, int toY, const std::vector<Portal>& portals) const;
    bool isGameOver(const std::vector<Portal>& portals) const;
    std::string getWinner(const std::vector<Portal>& portals) const;
    bool isValidEnPassant(Piece* piece, int fromX, int fromY, int toX, int toY, const LastMove& lastMove) const;
private:
    bool bfsWithPortals(Piece* piece, int fromX, int fromY, int toX, int toY, const std::vector<Portal>& portals) const;
    bool isKingInCheck(const std::string& color, const std::vector<Portal>& portals) const;
    bool canKingEscape(const std::string& color, const std::vector<Portal>& portals) const;
    bool canPieceBlockCheck(const std::string& color, const std::vector<Portal>& portals) const;
    bool isCheckmate(const std::string& color, const std::vector<Portal>& portals) const;
    std::vector<std::pair<int, int>> getKingMoves(int kingX, int kingY) const;
    bool isSquareUnderAttack(int x, int y, const std::string& attackingColor, const std::vector<Portal>& portals) const;
};