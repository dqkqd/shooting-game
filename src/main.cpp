
#include "game.h"
#include "tiles/tilemap.h"

auto main() -> int {
  auto game = Game("Hello", 50 * 16, 40 * 16);

  auto world = World();

  TileMap tile_map("assets/config/background.json");
  tile_map.init(game.graphic(), world);

  game.run(world);

  return 0;
}
