#include "Archer.h"
#include "ChessBoard.h"
#include <cmath>

Archer::Archer(const std::string& color) : Piece(color) {}

bool Archer::isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard* board) const {
    // Taşın kendi rengindeki başka bir taşın üzerine gitmesini engelle
    Piece* targetPiece = board->getPieceAt(toX, toY);
    if (targetPiece && targetPiece->getColor() == this->getColor()) {
        return false;
    }

    int dx = toX - fromX;
    int dy = toY - fromY;
    int absDx = std::abs(dx);
    int absDy = std::abs(dy);

    // Dikey, yatay veya çapraz hareket
    if (absDx == absDy || dx == 0 || dy == 0) {
        // Hareket mesafesi kontrolü (1 veya 2 kare)
        if (absDx <= 2 && absDy <= 2) {
            // Yol üzerindeki taşları kontrol et
            int stepX = (dx == 0) ? 0 : dx / absDx;
            int stepY = (dy == 0) ? 0 : dy / absDy;
            
            int x = fromX + stepX;
            int y = fromY + stepY;
            
            // Hedef noktaya kadar olan yolu kontrol et
            while (x != toX || y != toY) {
                Piece* piece = board->getPieceAt(x, y);
                if (piece) {
                    // Kendi rengindeki taşların üzerinden atlayabilir
                    if (piece->getColor() != this->getColor()) {
                        return false;
                    }
                }
                x += stepX;
                y += stepY;
            }
            return true;
        }
    }
    return false;
}

bool Archer::canAttack(int fromX, int fromY, int toX, int toY, ChessBoard* board) const {
    // Saldırı mesafesi kontrolü (tam olarak 2 kare)
    int dx = toX - fromX;
    int dy = toY - fromY;
    int absDx = std::abs(dx);
    int absDy = std::abs(dy);

    if ((absDx == 2 && absDy == 0) || // Yatay saldırı
        (absDx == 0 && absDy == 2) || // Dikey saldırı
        (absDx == 2 && absDy == 2)) { // Çapraz saldırı
        
        // Hedefte rakip taş var mı kontrol et
        Piece* targetPiece = board->getPieceAt(toX, toY);
        if (targetPiece && targetPiece->getColor() != this->getColor()) {
            return true;
        }
    }
    return false;
} 