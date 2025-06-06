#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "Portal.h"
#include "Position.h"

// Forward declarations
struct Movement;
struct SpecialAbilities;
struct PieceConfig;
struct PortalProperties;
struct PortalConfig;
struct GameConfig;

// Movement capabilities for chess pieces
struct Movement {
  int forward = 0;
  int sideways = 0;
  int diagonal = 0;
  bool l_shape = false;
  int diagonal_capture = 0;
  int first_move_forward = 0;
};

// Special abilities for chess pieces
struct SpecialAbilities {
  bool castling = false;
  bool royal = false;
  bool jump_over = false;
  bool promotion = false;
  bool en_passant = false;
  std::unordered_map<std::string, bool> custom_abilities;
};

// Configuration for a chess piece
struct PieceConfig {
  std::string type;
  std::unordered_map<std::string, std::vector<Position>> positions;
  Movement movement;
  SpecialAbilities special_abilities;
  int count;
};

// Properties for portals
struct PortalProperties {
  bool preserve_direction;
  std::vector<std::string> allowed_colors;
  int cooldown;
};

// Configuration for a portal
struct PortalConfig {
  std::string type;
  std::string id;
  struct {
    Position entry;
    Position exit;
  } positions;
  PortalProperties properties;
};

// Game configuration
struct GameConfig {
  struct {
    std::string name;
    int board_size;
    int turn_limit;
  } game_settings;

  std::vector<PieceConfig> pieces;
  std::vector<PieceConfig> custom_pieces;
  std::vector<PortalConfig> portals;
};

class ConfigReader {
public:
  // Constructor
  ConfigReader();

  // Load configuration from a file
  bool loadFromFile(const std::string &filePath);

  // Load configuration from a JSON string
  bool loadFromString(const std::string &jsonString);

  // Get the parsed configuration
  const GameConfig &getConfig() const;

  // Validate the configuration
  bool validateConfig();

  // Get portals from the configuration
  const std::vector<PortalConfig>& getPortals() const;

private:
  GameConfig m_config;

  void parseGameSettings(const nlohmann::json &json);
  void parsePieces(const nlohmann::json &json);
  void parseCustomPieces(const nlohmann::json &json);
  void parsePortals(const nlohmann::json &json);
  void parseSpecialAbilities(const nlohmann::json &abilities,
                             SpecialAbilities &specialAbilities);
};