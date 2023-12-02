
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
  event_systems.add(SystemType::PARALLEL, player::test_player_event);

  SystemManager normal_systems;
  normal_systems.add(SystemType::PARALLEL, player::animation_system);
  normal_systems.add(SystemType::PARALLEL, player::moving_system);
  normal_systems.add(SystemType::SEQUENTIAL, player::camera_system,
                     game.graphic().camera());
  normal_systems.add(SystemType::SEQUENTIAL, shared_systems::render_system,
                     game.graphic());

  TileMap tile_map(BACKGROUND_CONFIG_TILEMAP);
  tile_map.init(game.graphic(), world);

  player::init(game.graphic(), world);

  game.run(world, event_systems, normal_systems);

  return 0;
}
