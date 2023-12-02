
#include "config.h"
#include "ecs/systems/core.h"
#include "game.h"
#include "player.h"
#include "systems.h"
#include "tiles/tilemap.h"

auto main() -> int {
  auto game = Game("Hello", GAME_WIDTH, GAME_HEIGHT);

  auto world = World();

  sys::SystemManager<SDL_Event> event_systems;
  event_systems.add(sys::SystemType::PARALLEL, player::test_player_event);

  TileMap tile_map(BACKGROUND_CONFIG_TILEMAP);
  tile_map.init(game.graphic(), world);

  player::init(game.graphic(), world);

  world.add_parallel_system(player::animation_system)
      .add_parallel_system(player::moving_system)
      .add_system(player::camera_system, game.graphic().camera())
      .add_system(shared_systems::render_system, game.graphic());

  game.run(world, event_systems);

  return 0;
}
