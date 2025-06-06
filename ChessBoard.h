#pragma once
#include <unordered_map>
#include <string>
#include "Piece.h"
#include <vector>

class ChessBoard {
private:
    std::unordered_map<std::string, Piece*> board;
    int size;

    std::string posToKey(int x, int y) const;

public:
    ChessBoard(int boardSize);
    bool isValidPosition(int x, int y) const;
    Piece* getPieceAt(int x, int y) const;
    void placePiece(int x, int y, Piece* piece);
    void removePiece(int x, int y);
    void movePiece(int fromX, int fromY, int toX, int toY);
    std::vector<Piece*> getAllPieces() const;
};