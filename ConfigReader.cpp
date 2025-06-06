#include "ConfigReader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "Position.h"

using json = nlohmann::json;

ConfigReader::ConfigReader() {}

bool ConfigReader::loadFromFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    json jsonData;
    file >> jsonData;
    file.close();

    parseGameSettings(jsonData);
    parsePieces(jsonData);
    parseCustomPieces(jsonData);
    parsePortals(jsonData);  // 💡 yeni eklendi

    return true;
}

bool ConfigReader::loadFromString(const std::string &jsonString) {
    json jsonData = json::parse(jsonString);

    parseGameSettings(jsonData);
    parsePieces(jsonData);
    parseCustomPieces(jsonData);
    parsePortals(jsonData);

    return true;
}

const GameConfig &ConfigReader::getConfig() const {
    return m_config;
}

bool ConfigReader::validateConfig() {
    // Basit kontrol: en az bir taş ve board_size > 0 olsun
    return m_config.pieces.size() > 0 && m_config.game_settings.board_size > 0;
}

const std::vector<PortalConfig>& ConfigReader::getPortals() const {
    return m_config.portals;
}

void ConfigReader::parseGameSettings(const json &jsonData) {
    const auto &settings = jsonData["game_settings"];
    m_config.game_settings.name = settings["name"];
    m_config.game_settings.board_size = settings["board_size"];
    m_config.game_settings.turn_limit = settings["turn_limit"];
}

void ConfigReader::parsePieces(const json &jsonData) {
    m_config.pieces.clear();
    for (const auto &piece : jsonData["pieces"]) {
        PieceConfig config;
        config.type = piece["type"];
        config.count = piece["count"];

        for (const auto &side : piece["positions"].items()) {
            std::string color = side.key();
            for (const auto &pos : side.value()) {
                config.positions[color].push_back({pos["x"], pos["y"]});
            }
        }

        if (piece.contains("movement")) {
            const auto &mv = piece["movement"];
            if (mv.contains("forward")) config.movement.forward = mv["forward"];
            if (mv.contains("sideways")) config.movement.sideways = mv["sideways"];
            if (mv.contains("diagonal")) config.movement.diagonal = mv["diagonal"];
            if (mv.contains("l_shape")) config.movement.l_shape = mv["l_shape"];
            if (mv.contains("diagonal_capture")) config.movement.diagonal_capture = mv["diagonal_capture"];
            if (mv.contains("first_move_forward")) config.movement.first_move_forward = mv["first_move_forward"];
        }

        if (piece.contains("special_abilities")) {
            parseSpecialAbilities(piece["special_abilities"], config.special_abilities);
        }

        m_config.pieces.push_back(config);
    }
}

void ConfigReader::parseCustomPieces(const json &jsonData) {
    // Aynı yapı parçacığını kullanabiliriz
    // Gerekirse ayrı işlenebilir
    m_config.custom_pieces.clear();
    // Şimdilik boş
}

void ConfigReader::parseSpecialAbilities(const json &abilitiesJson, SpecialAbilities &abilities) {
    if (abilitiesJson.contains("castling")) abilities.castling = abilitiesJson["castling"];
    if (abilitiesJson.contains("royal")) abilities.royal = abilitiesJson["royal"];
    if (abilitiesJson.contains("jump_over")) abilities.jump_over = abilitiesJson["jump_over"];
    if (abilitiesJson.contains("promotion")) abilities.promotion = abilitiesJson["promotion"];
    if (abilitiesJson.contains("en_passant")) abilities.en_passant = abilitiesJson["en_passant"];

    for (auto it = abilitiesJson.begin(); it != abilitiesJson.end(); ++it) {
        std::string key = it.key();
        if (key != "castling" && key != "royal" && key != "jump_over" &&
            key != "promotion" && key != "en_passant") {
            abilities.custom_abilities[key] = it.value();
        }
    }
}

void ConfigReader::parsePortals(const json &jsonData) {
    m_config.portals.clear();
    if (!jsonData.contains("portals")) return;

    for (const auto &portalJson : jsonData["portals"]) {
        PortalConfig config;
        config.id = portalJson["id"];
        config.positions.entry = { portalJson["positions"]["entry"]["x"], portalJson["positions"]["entry"]["y"] };
        config.positions.exit = { portalJson["positions"]["exit"]["x"], portalJson["positions"]["exit"]["y"] };

        config.properties.preserve_direction = portalJson["properties"]["preserve_direction"];
        config.properties.cooldown = portalJson["properties"]["cooldown"];
        
        for (const auto &c : portalJson["properties"]["allowed_colors"]) {
            config.properties.allowed_colors.push_back(c);
        }

        m_config.portals.push_back(config);
    }
}