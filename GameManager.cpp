#include "GameManager.h"
#include <iostream>

GameManager::GameManager() : gameOver(false), currentPlayer("white") {
    board = new ChessBoard(8);
    validator = new MoveValidator(board);
    printer = new BoardPrinter(board);
    // Initialize lastMove
    lastMove = {0, 0, 0, 0, ""};
}

GameManager::~GameManager() {
    delete board;
    delete validator;
    delete printer;
}

void GameManager::start() {
    ConfigReader::loadConfig("chess_pieces.json", board);
    printBoard();
}

void GameManager::makeMove(int fromX, int fromY, int toX, int toY) {
    if (gameOver) return;

    Piece* piece = board->getPieceAt(fromX, fromY);
    if (!piece || piece->getColor() != currentPlayer) return;

    std::cout << "Attempting move: " << fromX << "," << fromY << " -> " << toX << "," << toY << std::endl;
    std::cout << "Piece type: " << piece->getType() << ", Color: " << piece->getColor() << std::endl;

    // Validate move
    if (!validator->validateMove(piece, fromX, fromY, toX, toY, {})) {
        std::cout << "Invalid move!" << std::endl;
        return;
    }

    // Check for adjacent pawn capture
    if (piece->getType() == "Pawn" && std::abs(toX - fromX) == 1 && std::abs(toY - fromY) == 1) {
        Piece* targetPiece = board->getPieceAt(toX, toY);
        if (!targetPiece) {
            // This is an adjacent pawn capture
            // Check the square where the pawn was passed
            int passedPawnY = (toY > fromY) ? fromY : toY; // Use the lower Y coordinate
            Piece* passedPawn = board->getPieceAt(toX, passedPawnY);
            if (passedPawn && passedPawn->getType() == "Pawn" && passedPawn->getColor() != piece->getColor()) {
                std::cout << "Capturing passed pawn at " << toX << "," << passedPawnY << std::endl;
                board->removePiece(toX, passedPawnY);
            }
        }
    }

    // Store move information
    lastMove = {fromX, fromY, toX, toY, piece->getType()};

    // Make the move
    board->movePiece(fromX, fromY, toX, toY);
    moveHistory.push({{fromX, fromY}, {toX, toY}});

    // Check for pawn promotion
    if (piece->getType() == "Pawn" && (toY == 0 || toY == 7)) {
        // Promote to queen (you can add a method to let the player choose the piece)
        board->removePiece(toX, toY);
        board->placePiece(toX, toY, new Piece("Queen", currentPlayer, {{"forward", 8}, {"sideways", 8}, {"diagonal", 8}}, {}));
    }

    switchPlayer();
}

void GameManager::undoMove() {
    if (moveHistory.empty()) return;

    auto lastMove = moveHistory.top();
    moveHistory.pop();

    int fromX = lastMove.first.first;
    int fromY = lastMove.first.second;
    int toX = lastMove.second.first;
    int toY = lastMove.second.second;

    // Check if it was an en passant capture
    Piece* movedPiece = board->getPieceAt(toX, toY);
    if (movedPiece && movedPiece->getType() == "Pawn" && std::abs(toX - fromX) == 1 && std::abs(toY - fromY) == 1) {
        // Restore the captured pawn
        std::string capturedColor = (movedPiece->getColor() == "white") ? "black" : "white";
        board->placePiece(toX, fromY, new Piece("Pawn", capturedColor, {{"forward", 1}}, {}));
    }

    board->movePiece(toX, toY, fromX, fromY);
    switchPlayer();
}

bool GameManager::isGameOver() const {
    return gameOver;
}

std::string GameManager::getCurrentPlayer() const {
    return currentPlayer;
}

void GameManager::switchPlayer() {
    currentPlayer = (currentPlayer == "white") ? "black" : "white";
}

void GameManager::printBoard() const {
    printer->printBoard(board);
}

void GameManager::printGameStatus() const {
    std::cout << "Turn: " << currentPlayer << std::endl;
    if (validator->isKingInCheck(currentPlayer, board->getPortals())) {
        std::cout << "King is in check!" << std::endl;
    }
    if (gameOver) {
        std::cout << "Game over! Winner: " << validator->getWinner(board->getPortals()) << std::endl;
    }
} 