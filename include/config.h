#ifndef CONFIG_H
#define CONFIG_H

#include <set>
#include <string>

/* game constants */
constexpr int GAME_WIDTH = 800;
constexpr int GAME_HEIGHT = 640;
constexpr float PIXEL_FLOAT_OFFSET = 1e-5;

constexpr int CAMERA_WIDTH = GAME_WIDTH;
constexpr int CAMERA_HEIGHT = GAME_HEIGHT;

constexpr int LEVEL_WIDTH = 64 * 16;
constexpr int LEVEL_HEIGHT = 40 * 16;

/* assets constants */
constexpr char const* ASSETS_CONFIG_FOLDER = "assets/config/";
constexpr char const* BACKGROUND_CONFIG_TILEMAP =
    "assets/config/background.json";

/* player constants */
constexpr char const* PLAYER_IMAGE =
    "assets/pixel_adventure_assets/Enemies/Slime/Idle-Run (44x30).png";
constexpr int PLAYER_TOTAL_SPRITES = 10;
constexpr int PLAYER_FRAMES_DELAY = 3;
constexpr int PLAYER_SPRITE_WIDTH = 44;
constexpr int PLAYER_SPRITE_HEIGHT = 30;

/* physics constants */
constexpr float PHYSIC_SCALE = 10000;
constexpr float DEFAULT_GRAVITY = 9.8;
constexpr float TICK_OFFSET = 1.0 / 60;

const std::set<std::string> COLLIDABLE_TILESETS{"Terrain"};  // NOLINT
#endif
