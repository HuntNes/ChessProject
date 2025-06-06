#include "MoveValidator.h"
#include <cmath>
#include <queue>
#include <set>

MoveValidator::MoveValidator(ChessBoard* b) : board(b) {}

bool MoveValidator::isValidLinearMove(int fromX, int fromY, int toX, int toY) const {
    return (fromX == toX || fromY == toY || abs(fromX - toX) == abs(fromY - toY));
}

bool MoveValidator::isPathClear(int fromX, int fromY, int toX, int toY) const {
    int dx = (toX - fromX == 0) ? 0 : (toX - fromX) / std::abs(toX - fromX);
    int dy = (toY - fromY == 0) ? 0 : (toY - fromY) / std::abs(toY - fromY);

    int x = fromX + dx;
    int y = fromY + dy;

    while (x != toX || y != toY) {
        if (board->getPieceAt(x, y) != nullptr)
            return false;
        x += dx;
        y += dy;
    }

    return true;
}

bool MoveValidator::validateMove(Piece* piece, int fromX, int fromY, int toX, int toY, const std::vector<Portal>& portals) const {
    if (!piece) return false;
    std::string type = piece->getType();
    std::string color = piece->getColor();
    int dx = toX - fromX;
    int dy = toY - fromY;
    int absDx = std::abs(dx);
    int absDy = std::abs(dy);
    Piece* target = board->getPieceAt(toX, toY);

    // PAWN
    if (type == "Pawn") {
        int direction = (color == "white") ? 1 : -1;
        // İleri düz (taşsız)
        if (dx == 0 && dy == direction && !target) return true;
        // İlk hamlede iki kare ileri (taşsız)
        if (dx == 0 && dy == 2*direction && !target &&
            ((color == "white" && fromY == 1) || (color == "black" && fromY == 6)) &&
            !board->getPieceAt(fromX, fromY + direction)) return true;
        // Çapraz alma
        if (absDx == 1 && dy == direction && target && target->getColor() != color) return true;
        return false;
    }
    // KING
    if (type == "King") {
        // Sadece 1 kare her yöne hareket
        if (absDx <= 1 && absDy <= 1) return true;
        // Portal için BFS (rok hariç)
        if (absDx > 1 || absDy > 1) {
            // Portal kontrolü
            for (const auto& portal : portals) {
                if (portal.isAvailable() && portal.isColorAllowed(color)) {
                    Position entry = portal.getEntry();
                    Position exit = portal.getExit();
                    if ((fromX == entry.x && fromY == entry.y && toX == exit.x && toY == exit.y) ||
                        (fromX == exit.x && fromY == exit.y && toX == entry.x && toY == entry.y)) {
                        return true;
                    }
                }
            }
            return false;
        }
        return true;
    }
    // QUEEN
    if (type == "Queen") {
        if ((absDx == absDy || dx == 0 || dy == 0) && isPathClear(fromX, fromY, toX, toY)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    // ROOK
    if (type == "Rook") {
        if ((dx == 0 || dy == 0) && isPathClear(fromX, fromY, toX, toY)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    // BISHOP
    if (type == "Bishop") {
        if ((absDx == absDy) && isPathClear(fromX, fromY, toX, toY)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    // KNIGHT
    if (type == "Knight") {
        if ((absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    // Diğer taşlar ve özel yetenekler için BFS
    return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
}

bool MoveValidator::bfsWithPortals(Piece* piece, int fromX, int fromY, int toX, int toY, const std::vector<Portal>& portals) const {
    struct Node { int x, y; };
    std::queue<Node> q;
    std::set<std::pair<int, int>> visited;
    q.push({fromX, fromY});
    visited.insert({fromX, fromY});

    const auto movement = piece->getMovement();
    std::string color = piece->getColor();

    while (!q.empty()) {
        Node curr = q.front(); q.pop();
        if (curr.x == toX && curr.y == toY) return true;

        // Standart hamleler (dikey, yatay, çapraz, L)
        std::vector<std::pair<int, int>> directions = {
            {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {-1,-1}, {1,-1}, {-1,1}
        };
        bool canJump = piece->hasAbility("jump_over");
        for (const auto& dir : directions) {
            int maxStep = 0;
            if (dir.first == 0 || dir.second == 0) {
                if (movement.find("sideways") != movement.end())
                    maxStep = movement.at("sideways");
                if (movement.find("forward") != movement.end())
                    maxStep = std::max(maxStep, movement.at("forward"));
            } else {
                if (movement.find("diagonal") != movement.end())
                    maxStep = movement.at("diagonal");
            }
            for (int step = 1; step <= maxStep; ++step) {
                int nx = curr.x + dir.first * step;
                int ny = curr.y + dir.second * step;
                if (!board->isValidPosition(nx, ny)) break;
                if (!canJump && board->getPieceAt(nx, ny) != nullptr && (nx != toX || ny != toY)) break;
                if (visited.count({nx, ny}) == 0) {
                    q.push({nx, ny});
                    visited.insert({nx, ny});
                }
                if (!canJump && board->getPieceAt(nx, ny) != nullptr) break;
            }
        }
        // L-şekilli hareket
        if (movement.find("l_shape") != movement.end() && movement.at("l_shape")) {
            std::vector<std::pair<int, int>> l_moves = {
                {2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{1,-2},{2,-1}
            };
            for (const auto& mv : l_moves) {
                int nx = curr.x + mv.first;
                int ny = curr.y + mv.second;
                if (board->isValidPosition(nx, ny) && (board->getPieceAt(nx, ny) == nullptr || (nx == toX && ny == toY))) {
                    if (visited.count({nx, ny}) == 0) {
                        q.push({nx, ny});
                        visited.insert({nx, ny});
                    }
                }
            }
        }
        // Portal kullanımı
        for (const auto& portal : portals) {
            if (portal.isAvailable() && portal.isColorAllowed(color)) {
                Position entry = portal.getEntry();
                Position exit = portal.getExit();
                if (curr.x == entry.x && curr.y == entry.y) {
                    if (visited.count({exit.x, exit.y}) == 0) {
                        q.push({exit.x, exit.y});
                        visited.insert({exit.x, exit.y});
                    }
                }
            }
        }
    }
    return false;
}