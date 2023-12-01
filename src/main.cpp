
#include "config.h"
#include "game.h"
#include "player.h"
#include "tiles/tilemap.h"

auto main() -> int {
  auto game = Game("Hello", GAME_WIDTH, GAME_HEIGHT);

  auto world = World();

  TileMap tile_map(BACKGROUND_CONFIG_TILEMAP);
  tile_map.init(game.graphic(), world);

  player::init(game.graphic(), world);

  game.run(world);

  return 0;
}
