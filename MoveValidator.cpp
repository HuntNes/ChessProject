#include "MoveValidator.h"
#include "GameManager.h"
#include <cmath>
#include <queue>
#include <set>
#include <iostream>

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

bool MoveValidator::isValidEnPassant(Piece* piece, int fromX, int fromY, int toX, int toY, const LastMove& lastMove) const {
    if (!piece || piece->getType() != "Pawn") {
        std::cout << "Not a pawn" << std::endl;
        return false;
    }

    std::string color = piece->getColor();
    int direction = (color == "white") ? 1 : -1;

    std::cout << "Checking en passant:" << std::endl;
    std::cout << "Last move: " << lastMove.fromX << "," << lastMove.fromY << " -> " 
              << lastMove.toX << "," << lastMove.toY << " (" << lastMove.pieceType << ")" << std::endl;
    std::cout << "Current move: " << fromX << "," << fromY << " -> " << toX << "," << toY << std::endl;
    std::cout << "Direction: " << direction << std::endl;

    if (lastMove.pieceType != "Pawn") {
        std::cout << "Last move was not a pawn" << std::endl;
        return false;
    }

    int lastMoveDirection = (lastMove.toY > lastMove.fromY) ? 1 : -1;
    if (std::abs(lastMove.toY - lastMove.fromY) != 2) {
        std::cout << "Last move was not two squares" << std::endl;
        return false;
    }

    if (std::abs(lastMove.toX - fromX) != 1) {
        std::cout << "Pawns are not adjacent" << std::endl;
        return false;
    }

    if (lastMove.toY != fromY) {
        std::cout << "Pawns are not on the same rank" << std::endl;
        return false;
    }

    if (toX != lastMove.toX || toY != fromY + direction) {
        std::cout << "Target square is incorrect" << std::endl;
        return false;
    }

    std::cout << "En passant is valid!" << std::endl;
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

    if (type == "Pawn") {
        int direction = (color == "white") ? 1 : -1;
        if (dx == 0 && dy == direction && !target) return true;
        if (dx == 0 && dy == 2*direction && !target &&
            ((color == "white" && fromY == 1) || (color == "black" && fromY == 6)) &&
            !board->getPieceAt(fromX, fromY + direction)) return true;
        if (absDx == 1 && dy == direction) {
            if (target && target->getColor() != color) return true;
            Piece* adjacentPiece = board->getPieceAt(toX, fromY);
            if (adjacentPiece && adjacentPiece->getType() == "Pawn" && adjacentPiece->getColor() != color) {
                return true;
            }
        }
        return false;
    }

    if (type == "King") {
        if (absDx <= 1 && absDy <= 1) return true;
        if (absDx > 1 || absDy > 1) {
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
    if (type == "Queen") {
        if ((absDx == absDy || dx == 0 || dy == 0) && isPathClear(fromX, fromY, toX, toY)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    if (type == "Rook") {
        if ((dx == 0 || dy == 0) && isPathClear(fromX, fromY, toX, toY)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    if (type == "Bishop") {
        if ((absDx == absDy) && isPathClear(fromX, fromY, toX, toY)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
    if (type == "Knight") {
        if ((absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2)) return true;
        return bfsWithPortals(piece, fromX, fromY, toX, toY, portals);
    }
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

bool MoveValidator::isKingInCheck(const std::string& color, const std::vector<Portal>& portals) const {
    int kingX = -1, kingY = -1;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Piece* piece = board->getPieceAt(x, y);
            if (piece && piece->getType() == "King" && piece->getColor() == color) {
                kingX = x;
                kingY = y;
                break;
            }
        }
        if (kingX != -1) break;
    }

    std::string oppositeColor = (color == "white") ? "black" : "white";
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Piece* piece = board->getPieceAt(x, y);
            if (piece && piece->getColor() == oppositeColor) {
                if (validateMove(piece, x, y, kingX, kingY, portals)) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<std::pair<int, int>> MoveValidator::getKingMoves(int kingX, int kingY) const {
    std::vector<std::pair<int, int>> moves;
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; i++) {
        int newX = kingX + dx[i];
        int newY = kingY + dy[i];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            moves.emplace_back(newX, newY);
        }
    }
    return moves;
}

bool MoveValidator::isSquareUnderAttack(int x, int y, const std::string& attackingColor, const std::vector<Portal>& portals) const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->getPieceAt(i, j);
            if (piece && piece->getColor() == attackingColor) {
                if (validateMove(piece, i, j, x, y, portals)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool MoveValidator::canKingEscape(const std::string& color, const std::vector<Portal>& portals) const {
    int kingX = -1, kingY = -1;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Piece* piece = board->getPieceAt(x, y);
            if (piece && piece->getType() == "King" && piece->getColor() == color) {
                kingX = x;
                kingY = y;
                break;
            }
        }
        if (kingX != -1) break;
    }

    std::string oppositeColor = (color == "white") ? "black" : "white";
    for (const auto& move : getKingMoves(kingX, kingY)) {
        int newX = move.first;
        int newY = move.second;
        Piece* targetPiece = board->getPieceAt(newX, newY);

        if ((!targetPiece || targetPiece->getColor() != color) && 
            !isSquareUnderAttack(newX, newY, oppositeColor, portals)) {
            return true;
        }
    }
    return false;
}

bool MoveValidator::canPieceBlockCheck(const std::string& color, const std::vector<Portal>& portals) const {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Piece* piece = board->getPieceAt(x, y);
            if (piece && piece->getColor() == color) {
                for (int newX = 0; newX < 8; newX++) {
                    for (int newY = 0; newY < 8; newY++) {
                        if (validateMove(piece, x, y, newX, newY, portals)) {
                            Piece* capturedPiece = board->getPieceAt(newX, newY);
                            board->placePiece(newX, newY, piece);
                            board->removePiece(x, y);

                            bool stillInCheck = isKingInCheck(color, portals);

                            board->placePiece(x, y, piece);
                            if (capturedPiece) {
                                board->placePiece(newX, newY, capturedPiece);
                            } else {
                                board->removePiece(newX, newY);
                            }

                            if (!stillInCheck) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool MoveValidator::isCheckmate(const std::string& color, const std::vector<Portal>& portals) const {
    return isKingInCheck(color, portals) && !canKingEscape(color, portals) && !canPieceBlockCheck(color, portals);
}

bool MoveValidator::isGameOver(const std::vector<Portal>& portals) const {
    bool whiteKingExists = false;
    bool blackKingExists = false;
    
    for (const auto& piece : board->getAllPieces()) {
        if (piece->getType() == "King") {
            if (piece->getColor() == "white") {
                whiteKingExists = true;
            } else {
                blackKingExists = true;
            }
        }
    }
    
    return !whiteKingExists || !blackKingExists;
}

std::string MoveValidator::getWinner(const std::vector<Portal>& portals) const {
    bool whiteKingExists = false;
    bool blackKingExists = false;
    
    for (const auto& piece : board->getAllPieces()) {
        if (piece->getType() == "King") {
            if (piece->getColor() == "white") {
                whiteKingExists = true;
            } else {
                blackKingExists = true;
            }
        }
    }
    
    if (!whiteKingExists) return "black";
    if (!blackKingExists) return "white";
    return "draw";
}