#include <iostream>
#include <stack>
#include "ChessBoard.h"
#include "MoveValidator.h"
#include "Piece.h"
#include "ConfigReader.hpp"
#include "Position.h"
#include "BoardPrinter.h"

struct MoveRecord {
    int fromX, fromY, toX, toY;
    std::string pieceType;
    std::string pieceColor;
    Piece* capturedPiece = nullptr;
    bool usedPortal = false;
    Position portalExit;
};

int main() {
    ConfigReader reader;

    if (!reader.loadFromFile("chess_pieces.json")) {
        std::cerr << "JSON dosyasi yuklenemedi!\n";
        return 1;
    }

    const GameConfig& config = reader.getConfig();
    ChessBoard* board = new ChessBoard(config.game_settings.board_size);
    std::vector<Portal> portals;
    for (const auto& pc : reader.getPortals()) {
        portals.emplace_back(
            pc.id,
            pc.positions.entry,
            pc.positions.exit,
            pc.properties.preserve_direction,
            pc.properties.allowed_colors,
            pc.properties.cooldown
        );
    }

    for (const auto& pieceCfg : config.pieces) {
        for (const auto& [color, positions] : pieceCfg.positions) {
            for (const auto& pos : positions) {
                std::map<std::string, int> movementMap;
                if (pieceCfg.movement.forward > 0)
                    movementMap["forward"] = pieceCfg.movement.forward;
                if (pieceCfg.movement.sideways > 0)
                    movementMap["sideways"] = pieceCfg.movement.sideways;
                if (pieceCfg.movement.diagonal > 0)
                    movementMap["diagonal"] = pieceCfg.movement.diagonal;
                if (pieceCfg.movement.l_shape)
                    movementMap["l_shape"] = 1;

                std::map<std::string, bool> abilities(pieceCfg.special_abilities.custom_abilities.begin(),
                                                     pieceCfg.special_abilities.custom_abilities.end());
                abilities["castling"] = pieceCfg.special_abilities.castling;
                abilities["royal"] = pieceCfg.special_abilities.royal;
                abilities["jump_over"] = pieceCfg.special_abilities.jump_over;
                abilities["promotion"] = pieceCfg.special_abilities.promotion;
                abilities["en_passant"] = pieceCfg.special_abilities.en_passant;

                Piece* piece = new Piece(
                    std::string(pieceCfg.type),
                    std::string(color),
                    movementMap,
                    abilities
                );

                board->placePiece(pos.x, pos.y, piece);
            }
        }
    }

    // Custom pieces'i de tahtaya yerleştir
    for (const auto& pieceCfg : config.custom_pieces) {
        for (const auto& [color, positions] : pieceCfg.positions) {
            for (const auto& pos : positions) {
                std::map<std::string, int> movementMap;
                if (pieceCfg.movement.forward > 0)
                    movementMap["forward"] = pieceCfg.movement.forward;
                if (pieceCfg.movement.sideways > 0)
                    movementMap["sideways"] = pieceCfg.movement.sideways;
                if (pieceCfg.movement.diagonal > 0)
                    movementMap["diagonal"] = pieceCfg.movement.diagonal;
                if (pieceCfg.movement.l_shape)
                    movementMap["l_shape"] = 1;

                std::map<std::string, bool> abilities(pieceCfg.special_abilities.custom_abilities.begin(),
                                                     pieceCfg.special_abilities.custom_abilities.end());
                abilities["castling"] = pieceCfg.special_abilities.castling;
                abilities["royal"] = pieceCfg.special_abilities.royal;
                abilities["jump_over"] = pieceCfg.special_abilities.jump_over;
                abilities["promotion"] = pieceCfg.special_abilities.promotion;
                abilities["en_passant"] = pieceCfg.special_abilities.en_passant;

                Piece* piece = new Piece(
                    std::string(pieceCfg.type),
                    std::string(color),
                    movementMap,
                    abilities
                );

                board->placePiece(pos.x, pos.y, piece);
            }
        }
    }

    MoveValidator validator(board);
    std::stack<MoveRecord> moveHistory;
    BoardPrinter printer;
    std::cout << "\nCustom Chess started. Commands: move x1 y1 x2 y2 | undo | quit\n\n";

    std::string command;
    while (true) {
        printer.print(*board);
        std::cout << "> ";
        std::cin >> command;

        if (command == "quit" || command == "exit") {
            std::cout << "Game ended.\n";
            break;
        } else if (command == "undo") {
            if (moveHistory.empty()) {
                std::cout << "No move to undo!\n";
                continue;
            }
            MoveRecord last = moveHistory.top();
            moveHistory.pop();

            Piece* piece = board->getPieceAt(last.toX, last.toY);
            if (last.usedPortal) {
                board->movePiece(last.portalExit.x, last.portalExit.y, last.fromX, last.fromY);
            } else {
                board->movePiece(last.toX, last.toY, last.fromX, last.fromY);
            }
            if (last.capturedPiece) {
                board->placePiece(last.toX, last.toY, last.capturedPiece);
            }
            std::cout << "Last move undone!\n";
            continue;
        } else if (command == "move") {
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;

            Piece* piece = board->getPieceAt(x1, y1);
            if (!piece) {
                std::cout << "No piece at that position!\n";
                continue;
            }

            if (validator.validateMove(piece, x1, y1, x2, y2, portals)) {
                Piece* captured = board->getPieceAt(x2, y2);
                board->movePiece(x1, y1, x2, y2);
                std::cout << piece->getType() << " moved!\n";

                bool usedPortal = false;
                Position portalExit;
                for (auto& portal : portals) {
                    Position entry = portal.getEntry();
                    Position exit = portal.getExit();
                    if (entry.x == x2 && entry.y == y2 && portal.isColorAllowed(piece->getColor())) {
                        if (portal.isAvailable()) {
                            std::cout << "Portal active: " << portal.getId() << " → piece is teleporting...\n";
                            board->movePiece(x2, y2, exit.x, exit.y);
                            std::cout << piece->getType() << " teleported via portal (" << x2 << "," << y2 << ") → ("
                                      << exit.x << "," << exit.y << ")\n";
                            portal.startCooldown();
                            usedPortal = true;
                            portalExit = exit;
                        } else {
                            std::cout << "Portal is on cooldown: " << portal.getId() << "\n";
                        }
                        break;
                    }
                }

                moveHistory.push({x1, y1, x2, y2, piece->getType(), piece->getColor(), captured, usedPortal, portalExit});
            } else {
                std::cout << "Invalid move!\n";
            }
        } else if (command == "attack") {
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            Piece* piece = board->getPieceAt(x1, y1);
            if (!piece) {
                std::cout << "No piece at that position!\n";
                continue;
            }
            if (!piece->hasAbility("ranged_attack")) {
                std::cout << "This piece does not have the ranged_attack ability!\n";
                continue;
            }
            int range = 1;
            auto movement = piece->getMovement();
            if (movement.find("attack_range") != movement.end())
                range = movement["attack_range"];
            int dx = abs(x2 - x1);
            int dy = abs(y2 - y1);
            if ((dx <= range && dy == 0) || (dy <= range && dx == 0) || (dx == dy && dx <= range)) {
                Piece* target = board->getPieceAt(x2, y2);
                if (target && target->getColor() != piece->getColor()) {
                    board->removePiece(x2, y2);
                    std::cout << piece->getType() << " performed a ranged attack and destroyed the enemy piece!\n";
                } else {
                    std::cout << "No enemy piece at the target!\n";
                }
            } else {
                std::cout << "Target is out of range!\n";
            }
            continue;
        } else {
            std::cout << "Unknown command!\n";
        }

        for (auto& portal : portals) {
            portal.decrementCooldown();
        }

        if (validator.isGameOver(portals)) {
            if (validator.getWinner(portals) == "white")
                std::cout << "White wins!\n";
            else if (validator.getWinner(portals) == "black")
                std::cout << "Black wins!\n";
            else
                std::cout << "Draw!\n";
            break;
        }
    }

    return 0;
}