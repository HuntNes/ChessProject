#include "BoardPrinter.h"
#include <iostream>
#include <unordered_map>

void BoardPrinter::print(const ChessBoard& board) const {
    std::unordered_map<std::string, std::string> whiteSymbols = {
        {"King", "♔"}, {"Queen", "♕"}, {"Rook", "♖"},
        {"Bishop", "♗"}, {"Knight", "♘"}, {"Pawn", "♙"}
    };
    std::unordered_map<std::string, std::string> blackSymbols = {
        {"King", "♚"}, {"Queen", "♛"}, {"Rook", "♜"},
        {"Bishop", "♝"}, {"Knight", "♞"}, {"Pawn", "♟"}
    };

    std::cout << "   ";
    for (int x = 0; x < 8; ++x) std::cout << " " << x;
    std::cout << "\n";
    for (int y = 7; y >= 0; --y) {
        std::cout << " " << y << " ";
        for (int x = 0; x < 8; ++x) {
            Piece* p = board.getPieceAt(x, y);
            std::string symbol = ".";
            if (p) {
                std::string t = p->getType();
                std::string color = p->getColor();
                if (color == "white" && whiteSymbols.count(t)) symbol = whiteSymbols[t];
                else if (color == "black" && blackSymbols.count(t)) symbol = blackSymbols[t];
                else symbol = t.substr(0, 1);
            }
            std::cout << " " << symbol;
        }
        std::cout << "\n";
    }
}