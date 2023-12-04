#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <set>
#include <string>

using json = nlohmann::json;

namespace config {

struct Graphic {
  int width;
  int height;
  float pixel_offset;
};

struct Level {
  int width;
  int height;
};

struct Camera {
  int width;
  int height;
};

struct Physics {
  float gravity;
  float dt;
};

struct Assets {
  std::filesystem::path folder;
  std::filesystem::path config_folder;
  std::filesystem::path images_folder;
};

struct TileMap {
  std::filesystem::path background;
  std::set<std::string> collidable;
};

struct Position {
  int x;
  int y;
};
struct Character {
  std::filesystem::path image;
  int total_sprites;
  int frames_delay;
  int width;
  int height;
  std::vector<Position> positions;
};

struct Player : public Character {
  struct Shooter {
    float velocity_scale;
    float max_velocity;
    float dt;
  } shooter;
};

struct Rhino : public Character {
  float speed;
  std::vector<Position> from;
  std::vector<Position> to;
};

struct Enemy {
  Rhino rhino;
};

struct Game {
  Graphic graphic{};
  Level level{};
  Camera camera{};
  Physics physics{};
  Assets assets;
  TileMap tile_map;
  Player player;

  Enemy enemy;

  static auto from_config(const char* file) -> Game;
};

inline void from_json(const json& j, Game& game) {
  j["graphic"]["width"].get_to(game.graphic.width);
  j["graphic"]["height"].get_to(game.graphic.height);
  j["graphic"]["pixelOffset"].get_to(game.graphic.pixel_offset);

  j["level"]["width"].get_to(game.level.width);
  j["level"]["height"].get_to(game.level.height);

  j["camera"]["width"].get_to(game.camera.width);
  j["camera"]["height"].get_to(game.camera.height);

  j["physics"]["gravity"].get_to(game.physics.gravity);
  j["physics"]["dt"].get_to(game.physics.dt);

  j["assets"]["folder"].get_to(game.assets.folder);
  game.assets.config_folder =
      game.assets.folder / j["assets"]["config"]["folder"];
  game.assets.images_folder =
      game.assets.folder / j["assets"]["images"]["folder"];

  game.tile_map.background =
      game.assets.config_folder / j["tileMap"]["background"];
  j["tileMap"]["collidable"].get_to(game.tile_map.collidable);

  game.player.image = game.assets.images_folder / j["player"]["image"];
  j["player"]["totalSprites"].get_to(game.player.total_sprites);
  j["player"]["framesDelay"].get_to(game.player.frames_delay);
  j["player"]["width"].get_to(game.player.width);
  j["player"]["height"].get_to(game.player.height);
  j["player"]["shooter"]["velocityScale"].get_to(
      game.player.shooter.velocity_scale);
  j["player"]["shooter"]["maxVelocity"].get_to(
      game.player.shooter.max_velocity);
  j["player"]["shooter"]["dt"].get_to(game.player.shooter.dt);

  game.enemy.rhino.image =
      game.assets.images_folder / j["enemy"]["rhino"]["image"];
  j["enemy"]["rhino"]["totalSprites"].get_to(game.enemy.rhino.total_sprites);
  j["enemy"]["rhino"]["framesDelay"].get_to(game.enemy.rhino.frames_delay);
  j["enemy"]["rhino"]["width"].get_to(game.enemy.rhino.width);
  j["enemy"]["rhino"]["height"].get_to(game.enemy.rhino.height);
  j["enemy"]["rhino"]["speed"].get_to(game.enemy.rhino.speed);
  for (const auto& elem : j["enemy"]["rhino"]["positions"]) {
    game.enemy.rhino.positions.emplace_back(Position{elem["x"], elem["y"]});
  }
  for (const auto& elem : j["enemy"]["rhino"]["from"]) {
    game.enemy.rhino.from.emplace_back(Position{elem["x"], elem["y"]});
  }
  for (const auto& elem : j["enemy"]["rhino"]["to"]) {
    game.enemy.rhino.to.emplace_back(Position{elem["x"], elem["y"]});
  }
}

};  // namespace config

class GameConfig {
 public:
  GameConfig(const GameConfig&) = delete;
  GameConfig(GameConfig&&) = delete;
  auto operator=(const GameConfig&) -> GameConfig& = delete;
  auto operator=(GameConfig&&) -> GameConfig& = delete;
  ~GameConfig() = delete;

  static auto data() -> config::Game& {
    std::ifstream f(CONFIG_FILE);
    auto parsed = json::parse(f);
    static config::Game instance = parsed.template get<config::Game>();

    return instance;
  }

 private:
  static constexpr const char* const CONFIG_FILE = "config.json";
};
#endif
