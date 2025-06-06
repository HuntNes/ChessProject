#pragma once
#include <string>
#include <map>

class Piece {
private:
    std::string type;
    std::string color;

    // Hareket kabiliyetleri: örn. forward = 1, l_shape = 1
    std::map<std::string, int> movement;

    // Özel yetenekler: örn. castling = true
    std::map<std::string, bool> specialAbilities;

public:
    // Yapıcı (constructor)
    Piece(const std::string& type,
          const std::string& color,
          const std::map<std::string, int>& movement,
          const std::map<std::string, bool>& abilities);

    // Getter fonksiyonları
    std::string getType() const;
    std::string getColor() const;
    std::map<std::string, int> getMovement() const;
    std::map<std::string, bool> getSpecialAbilities() const;

    // Yeteneğin varlığını kontrol et
    bool hasAbility(const std::string& key) const;
};