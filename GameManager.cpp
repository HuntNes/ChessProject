#include "GameManager.h"
#include <iostream>

GameManager::GameManager() : currentPlayer("white"), gameOver(false) {
    board = new ChessBoard();
    validator = new MoveValidator(board);
    printer = new BoardPrinter();
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
    if (gameOver) {
        std::cout << "Oyun bitti! Yeni bir oyun başlatın." << std::endl;
        return;
    }

    Piece* piece = board->getPieceAt(fromX, fromY);
    if (!piece) {
        std::cout << "Seçilen konumda taş yok!" << std::endl;
        return;
    }

    if (piece->getColor() != currentPlayer) {
        std::cout << "Sıra " << currentPlayer << " oyuncuda!" << std::endl;
        return;
    }

    if (!validator->validateMove(piece, fromX, fromY, toX, toY, board->getPortals())) {
        std::cout << "Geçersiz hamle!" << std::endl;
        return;
    }

    // Hamleyi yap
    Piece* capturedPiece = board->getPieceAt(toX, toY);
    board->setPieceAt(toX, toY, piece);
    board->setPieceAt(fromX, fromY, nullptr);

    // Hamleyi kaydet
    moveHistory.push({{fromX, fromY}, {toX, toY}});

    // Şah-mat kontrolü
    if (validator->isGameOver()) {
        gameOver = true;
        std::cout << "Şah-mat! " << validator->getWinner() << " oyuncu kazandı!" << std::endl;
    } else {
        switchPlayer();
    }

    printBoard();
    printGameStatus();
}

void GameManager::undoMove() {
    if (moveHistory.empty()) {
        std::cout << "Geri alınacak hamle yok!" << std::endl;
        return;
    }

    auto lastMove = moveHistory.top();
    moveHistory.pop();

    int fromX = lastMove.first.first;
    int fromY = lastMove.first.second;
    int toX = lastMove.second.first;
    int toY = lastMove.second.second;

    // Hamleyi geri al
    Piece* movedPiece = board->getPieceAt(toX, toY);
    board->setPieceAt(fromX, fromY, movedPiece);
    board->setPieceAt(toX, toY, nullptr);

    gameOver = false;
    switchPlayer();
    printBoard();
    printGameStatus();
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
    std::cout << "Sıra: " << currentPlayer << std::endl;
    if (validator->isKingInCheck(currentPlayer)) {
        std::cout << "Şah tehdit altında!" << std::endl;
    }
    if (gameOver) {
        std::cout << "Oyun bitti! Kazanan: " << validator->getWinner() << std::endl;
    }
} 