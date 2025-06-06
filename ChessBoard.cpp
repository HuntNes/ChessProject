#include "ChessBoard.h"
#include <sstream>

ChessBoard::ChessBoard(int boardSize) : size(boardSize) {}

std::string ChessBoard::posToKey(int x, int y) const {
    return std::to_string(x) + "," + std::to_string(y);
}

bool ChessBoard::isValidPosition(int x, int y) const {
    return x >= 0 && x < size && y >= 0 && y < size;
}

Piece* ChessBoard::getPieceAt(int x, int y) const {
    std::string key = posToKey(x, y);
    auto it = board.find(key);
    return (it != board.end()) ? it->second : nullptr;
}

void ChessBoard::placePiece(int x, int y, Piece* piece) {
    if (!isValidPosition(x, y)) return;
    board[posToKey(x, y)] = piece;
}

void ChessBoard::removePiece(int x, int y) {
    board.erase(posToKey(x, y));
}

void ChessBoard::movePiece(int fromX, int fromY, int toX, int toY) {
    Piece* piece = getPieceAt(fromX, fromY);
    if (piece && isValidPosition(toX, toY)) {
        removePiece(fromX, fromY);
        placePiece(toX, toY, piece);
    }
}

std::vector<Piece*> ChessBoard::getAllPieces() const {
    std::vector<Piece*> pieces;
    for (const auto& [key, piece] : board) {
        if (piece) pieces.push_back(piece);
    }
    return pieces;
}