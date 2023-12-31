#ifndef TILES_TILESET_H
#define TILES_TILESET_H

#include "components/position.h"
#include "ecs/world.h"
#include "graphic.h"
#include "tiles/parser.h"

class TileSet {
 public:
  explicit TileSet(parser::TileSetInMap data);

  void init(Graphic& graphic);
  auto texture() -> SDL_Texture*;
  auto data() -> parser::TileSetInMap;
  [[nodiscard]] auto collidable(int tile_id) const -> bool;
  [[nodiscard]] auto texture_info(int index) const -> TextureInfo;

 private:
  parser::TileSetInMap data_;
  SDL_Texture* texture_ = NULL;
};

class TileMap {
 public:
  explicit TileMap(const char* file);

  void init(World& world, Graphic& graphic);
  [[nodiscard]] auto render_info(int x, int y) const -> RenderInfo;

 private:
  parser::TileMap data_;
  std::map<int, TileSet> tilesets_;
};

#endif
