#pragma once
#include <string>
#include <map>

class Piece {
private:
    std::string type;
    std::string color;

    std::map<std::string, int> movement;

    std::map<std::string, bool> specialAbilities;

public:
    Piece(const std::string& type,
          const std::string& color,
          const std::map<std::string, int>& movement,
          const std::map<std::string, bool>& abilities);

    std::string getType() const;
    std::string getColor() const;
    std::map<std::string, int> getMovement() const;
    std::map<std::string, bool> getSpecialAbilities() const;

    bool hasAbility(const std::string& key) const;
};