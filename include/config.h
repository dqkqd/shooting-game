#ifndef CONFIG_H
#define CONFIG_H

/* game constants */
constexpr int GAME_WIDTH = 800;
constexpr int GAME_HEIGHT = 640;

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

#endif
