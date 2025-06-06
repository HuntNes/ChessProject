#include "Piece.h"

Piece::Piece(const std::string& type,
             const std::string& color,
             const std::map<std::string, int>& movement,
             const std::map<std::string, bool>& abilities)
    : type(type), color(color), movement(movement), specialAbilities(abilities) {}

std::string Piece::getType() const {
    return type;
}

std::string Piece::getColor() const {
    return color;
}

std::map<std::string, int> Piece::getMovement() const {
    return movement;
}

std::map<std::string, bool> Piece::getSpecialAbilities() const {
    return specialAbilities;
}

bool Piece::hasAbility(const std::string& key) const {
    auto it = specialAbilities.find(key);
    return it != specialAbilities.end() && it->second;
}