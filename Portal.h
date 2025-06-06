#pragma once
#include <string>
#include <vector>
#include "Position.h"

class Portal {
private:
    std::string id;
    Position entry;
    Position exit;
    bool preserveDirection;
    std::vector<std::string> allowedColors;
    int cooldown;
    int currentCooldown = 0;

public:
    Portal(std::string id, Position entry, Position exit,
           bool preserveDirection, std::vector<std::string> allowedColors, int cooldown);

    bool isAvailable() const;
    void startCooldown();
    void decrementCooldown();
    bool isColorAllowed(const std::string& color) const;

    Position getEntry() const;
    Position getExit() const;
    std::string getId() const;
};