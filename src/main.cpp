
#include "config.h"
#include "game.h"
#include "player.h"
#include "systems.h"
#include "tiles/tilemap.h"

auto main() -> int {
  auto game = Game("Hello", GAME_WIDTH, GAME_HEIGHT);

  auto world = World();

  TileMap tile_map(BACKGROUND_CONFIG_TILEMAP);
  tile_map.init(game.graphic(), world);

  player::init(game.graphic(), world);

  world.add_system(player::animation_system)
      .add_system(player::moving_system)
      .add_system(player::camera_system, game.graphic().camera())
      .add_system(shared_systems::render_system, game.graphic());

  game.run(world);

  return 0;
}
