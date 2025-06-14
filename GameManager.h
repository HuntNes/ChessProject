#ifndef CHESS3_GAMEMANAGER_H
#define CHESS3_GAMEMANAGER_H

#include "ConfigReader.hpp"
#include "ChessBoard.h"
#include "MoveValidator.h"
#include "BoardPrinter.h"
#include <stack>
#include <string>

// Store last move information for en passant
struct LastMove {
    int fromX, fromY, toX, toY;
    std::string pieceType;
};

class GameManager {
private:
    ChessBoard* board;
    MoveValidator* validator;
    BoardPrinter* printer;
    std::string currentPlayer;
    std::stack<std::pair<std::pair<int, int>, std::pair<int, int>>> moveHistory;
    bool gameOver;
    LastMove lastMove;

public:
    GameManager();
    ~GameManager();
    void start();
    void makeMove(int fromX, int fromY, int toX, int toY);
    void undoMove();
    bool isGameOver() const;
    std::string getCurrentPlayer() const;
    void switchPlayer();
    void printBoard() const;
    void printGameStatus() const;
    const LastMove& getLastMove() const { return lastMove; }
};

#endif //CHESS3_GAMEMANAGER_H 