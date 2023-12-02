
#include "config.h"
#include "ecs/system.h"
#include "game.h"
#include "player.h"
#include "systems.h"
#include "tiles/tilemap.h"

auto main() -> int {
  auto game = Game("Hello", GAME_WIDTH, GAME_HEIGHT);

  auto world = World();

  SystemManager<SDL_Event> event_systems;
  event_systems.add_parallel(player::test_player_event);

  SystemManager normal_systems;
  normal_systems.add_parallel(player::animation_system)
      .add_parallel(player::moving_system)
      .add_sequential(player::camera_system, game.graphic().camera())
      .add_sequential(shared_systems::render_system, game.graphic());

  TileMap tile_map(BACKGROUND_CONFIG_TILEMAP);
  tile_map.init(game.graphic(), world);

  player::init(game.graphic(), world);

  game.run(world, event_systems, normal_systems);

  return 0;
}
