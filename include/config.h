#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <set>

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
  std::string folder;
  std::string config_folder;
  std::string images_folder;
};

struct CollidableTiles {
  int gid;
  std::set<int> ids;
};
struct TileMap {
  std::string background;
  std::vector<CollidableTiles> collidables;
};

struct Position {
  int x;
  int y;
};

struct Sprite {
  std::string image;
  int total_sprites;
  int frames_delay;
  int width;
  int height;
};

struct SpriteDeadState : public Sprite {
  std::int64_t last{};
};

struct Goal : public Sprite {
  Position position;
};

struct Bullet : public Sprite {
  float scale{};
};

struct Player : public Sprite {
  struct Shooter {
    std::string indicator_image;
    float velocity_scale;
    float max_velocity;
    float dt;
  } shooter;
  Position position;
  SpriteDeadState dead_state;
  Bullet bullet;
};

struct Rhino : public Sprite {
  float speed;
  std::vector<Position> positions;
  std::vector<Position> from;
  std::vector<Position> to;
  SpriteDeadState dead_state;
};

struct Enemy {
  Rhino rhino;
};

struct GameState {
  std::string game_over_image;
  std::string game_finish_image;
};

struct Game {
  std::string game_title;
  Graphic graphic{};
  Level level{};
  Camera camera{};
  Physics physics{};
  Assets assets;
  TileMap tile_map;
  Player player;

  Enemy enemy;

  GameState game_state;
  Goal goal;

  static auto from_config(const char* file) -> Game;
};

inline void from_json(const json& j, Game& game) {
  j["gameTitle"].get_to(game.game_title);

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
      game.assets.folder + '/' +
      j["assets"]["config"]["folder"].get<std::string>();
  game.assets.images_folder =
      game.assets.folder + '/' +
      j["assets"]["images"]["folder"].get<std::string>();

  game.tile_map.background = game.assets.config_folder + '/' +
                             j["tileMap"]["background"].get<std::string>();
  for (const auto& elem : j["tileMap"]["collidables"]) {
    game.tile_map.collidables.emplace_back(
        CollidableTiles{.gid = elem["gid"], .ids = elem["ids"]});
  }

  game.player.image =
      game.assets.images_folder + '/' + j["player"]["image"].get<std::string>();
  j["player"]["totalSprites"].get_to(game.player.total_sprites);
  j["player"]["framesDelay"].get_to(game.player.frames_delay);
  j["player"]["width"].get_to(game.player.width);
  j["player"]["height"].get_to(game.player.height);

  game.player.dead_state.image =
      game.assets.images_folder + '/' +
      j["player"]["deadState"]["image"].get<std::string>();
  j["player"]["deadState"]["totalSprites"].get_to(
      game.player.dead_state.total_sprites);
  j["player"]["deadState"]["framesDelay"].get_to(
      game.player.dead_state.frames_delay);
  j["player"]["deadState"]["width"].get_to(game.player.dead_state.width);
  j["player"]["deadState"]["height"].get_to(game.player.dead_state.height);
  j["player"]["deadState"]["last"].get_to(game.player.dead_state.last);

  game.player.bullet.image = game.assets.images_folder + '/' +
                             j["player"]["bullet"]["image"].get<std::string>();
  j["player"]["bullet"]["totalSprites"].get_to(
      game.player.bullet.total_sprites);
  j["player"]["bullet"]["framesDelay"].get_to(game.player.bullet.frames_delay);
  j["player"]["bullet"]["width"].get_to(game.player.bullet.width);
  j["player"]["bullet"]["height"].get_to(game.player.bullet.height);
  j["player"]["bullet"]["scale"].get_to(game.player.bullet.scale);

  game.player.shooter.indicator_image =
      game.assets.images_folder + '/' +
      j["player"]["shooter"]["indicatorImage"].get<std::string>();
  j["player"]["shooter"]["velocityScale"].get_to(
      game.player.shooter.velocity_scale);
  j["player"]["shooter"]["maxVelocity"].get_to(
      game.player.shooter.max_velocity);
  j["player"]["shooter"]["dt"].get_to(game.player.shooter.dt);
  for (const auto& elem : j["player"]["positions"]) {
    game.player.position = Position{elem["x"], elem["y"]};
  }

  game.enemy.rhino.image = game.assets.images_folder + '/' +
                           j["enemy"]["rhino"]["image"].get<std::string>();
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
  game.enemy.rhino.dead_state.image =
      game.assets.images_folder + '/' +
      j["enemy"]["rhino"]["deadState"]["image"].get<std::string>();
  j["enemy"]["rhino"]["deadState"]["totalSprites"].get_to(
      game.enemy.rhino.dead_state.total_sprites);
  j["enemy"]["rhino"]["deadState"]["framesDelay"].get_to(
      game.enemy.rhino.dead_state.frames_delay);
  j["enemy"]["rhino"]["deadState"]["width"].get_to(
      game.enemy.rhino.dead_state.width);
  j["enemy"]["rhino"]["deadState"]["height"].get_to(
      game.enemy.rhino.dead_state.height);
  j["enemy"]["rhino"]["deadState"]["last"].get_to(
      game.enemy.rhino.dead_state.last);

  game.game_state.game_over_image =
      game.assets.images_folder + '/' +
      j["gameState"]["gameOverImage"].get<std::string>();
  game.game_state.game_finish_image =
      game.assets.images_folder + '/' +
      j["gameState"]["gameFinishImage"].get<std::string>();

  game.goal.image =
      game.assets.images_folder + '/' + j["goal"]["image"].get<std::string>();
  j["goal"]["totalSprites"].get_to(game.goal.total_sprites);
  j["goal"]["framesDelay"].get_to(game.goal.frames_delay);
  j["goal"]["width"].get_to(game.goal.width);
  j["goal"]["height"].get_to(game.goal.height);
  j["goal"]["height"].get_to(game.goal.height);
  j["goal"]["position"]["x"].get_to(game.goal.position.x);
  j["goal"]["position"]["y"].get_to(game.goal.position.y);
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
