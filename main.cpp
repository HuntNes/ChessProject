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
    std::cout << "\nCustom Chess baslatildi. Komutlar: move x1 y1 x2 y2 | undo | quit\n\n";

    std::string command;
    while (true) {
        printer.print(*board);
        std::cout << "> ";
        std::cin >> command;

        if (command == "quit" || command == "exit") {
            std::cout << "Oyun sonlandirildi.\n";
            break;
        } else if (command == "undo") {
            if (moveHistory.empty()) {
                std::cout << "Geri alinacak hamle yok!\n";
                continue;
            }
            MoveRecord last = moveHistory.top();
            moveHistory.pop();
            // Taşı geri al
            Piece* piece = board->getPieceAt(last.toX, last.toY);
            if (last.usedPortal) {
                // Portal ile taşınmışsa önce portal çıkışından geri al
                board->movePiece(last.portalExit.x, last.portalExit.y, last.fromX, last.fromY);
            } else {
                board->movePiece(last.toX, last.toY, last.fromX, last.fromY);
            }
            // Yakalanan taş varsa geri koy
            if (last.capturedPiece) {
                board->placePiece(last.toX, last.toY, last.capturedPiece);
            }
            std::cout << "Son hamle geri alindi!\n";
            continue;
        } else if (command == "move") {
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;

            Piece* piece = board->getPieceAt(x1, y1);
            if (!piece) {
                std::cout << "Orada tas yok!\n";
                continue;
            }

            if (validator.validateMove(piece, x1, y1, x2, y2, portals)) {
                Piece* captured = board->getPieceAt(x2, y2);
                board->movePiece(x1, y1, x2, y2);
                std::cout << piece->getType() << " hamle yapti!\n";

                // 🎯 PORTAL kontrolü
                bool usedPortal = false;
                Position portalExit;
                for (auto& portal : portals) {
                    Position entry = portal.getEntry();
                    Position exit = portal.getExit();
                    if (entry.x == x2 && entry.y == y2 && portal.isColorAllowed(piece->getColor())) {
                        if (portal.isAvailable()) {
                            std::cout << "Portal aktif: " << portal.getId() << " → tas isiniyor...\n";
                            board->movePiece(x2, y2, exit.x, exit.y);
                            std::cout << piece->getType() << " portalla (" << x2 << "," << y2 << ") → ("
                                      << exit.x << "," << exit.y << ") gecti.\n";
                            portal.startCooldown();
                            usedPortal = true;
                            portalExit = exit;
                        } else {
                            std::cout << "Portal cooldown'da: " << portal.getId() << "\n";
                        }
                        break;
                    }
                }
                // Hamle kaydını ekle
                moveHistory.push({x1, y1, x2, y2, piece->getType(), piece->getColor(), captured, usedPortal, portalExit});
            } else {
                std::cout << "Gecersiz hamle!\n";
            }
        } else if (command == "attack") {
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            Piece* piece = board->getPieceAt(x1, y1);
            if (!piece) {
                std::cout << "Orada tas yok!\n";
                continue;
            }
            if (!piece->hasAbility("ranged_attack")) {
                std::cout << "Bu tas ranged_attack yetenegine sahip degil!\n";
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
                    std::cout << piece->getType() << " uzaktan ates etti ve rakip tasi yok etti!\n";
                } else {
                    std::cout << "Hedefte rakip tas yok!\n";
                }
            } else {
                std::cout << "Hedef menzil disinda!\n";
            }
            continue;
        } else {
            std::cout << "Bilinmeyen komut!\n";
        }

        // Her hamle sonunda tüm portalların cooldown'unu azalt
        for (auto& portal : portals) {
            portal.decrementCooldown();
        }
        // Oyun sonu kontrolü: royal taşlardan biri kalmadıysa bitir
        int whiteRoyal = 0, blackRoyal = 0;
        for (auto* piece : board->getAllPieces()) {
            if (piece->hasAbility("royal")) {
                if (piece->getColor() == "white") whiteRoyal++;
                if (piece->getColor() == "black") blackRoyal++;
            }
        }
        if (whiteRoyal == 0 || blackRoyal == 0) {
            if (whiteRoyal == 0 && blackRoyal == 0)
                std::cout << "Oyun berabere!\n";
            else if (whiteRoyal == 0)
                std::cout << "Siyah kazandi!\n";
            else
                std::cout << "Beyaz kazandi!\n";
            break;
        }
    }

    return 0;
}