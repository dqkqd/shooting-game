
#include "game.h"
#include "tilemap.h"

auto main() -> int {
  auto game = Game("Hello", 640, 480);

  auto world = World();

  TileMap::init(game.graphic(), world);

  game.run(world);

  return 0;
}
