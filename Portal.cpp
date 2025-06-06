#include "Portal.h"

Portal::Portal(std::string id, Position entry, Position exit,
               bool preserveDirection, std::vector<std::string> allowedColors, int cooldown)
    : id(id), entry(entry), exit(exit), preserveDirection(preserveDirection),
      allowedColors(allowedColors), cooldown(cooldown) {}

bool Portal::isAvailable() const {
    return currentCooldown == 0;
}

void Portal::startCooldown() {
    currentCooldown = cooldown;
}

void Portal::decrementCooldown() {
    if (currentCooldown > 0) currentCooldown--;
}

bool Portal::isColorAllowed(const std::string& color) const {
    for (const auto& allowed : allowedColors) {
        if (allowed == color) return true;
    }
    return false;
}

Position Portal::getEntry() const { return entry; }
Position Portal::getExit() const { return exit; }
std::string Portal::getId() const { return id; }