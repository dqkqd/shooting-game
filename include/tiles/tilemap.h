#ifndef TILES_TILESET_H
#define TILES_TILESET_H

#include "components/primitive.h"
#include "ecs/world.h"
#include "graphic.h"
#include "tiles/parser.h"

struct CollidableTile {};

class TileSet {
 public:
  explicit TileSet(parser::TileSetInMap data);

  void init(Graphic& graphic);
  auto texture() -> SDL_Texture*;
  auto data() -> parser::TileSetInMap;
  [[nodiscard]] auto collidable() const -> bool;
  [[nodiscard]] auto texture_position(int index) const -> TexturePosition;

 private:
  parser::TileSetInMap data_;
  SDL_Texture* texture_ = NULL;
};

class TileMap {
 public:
  explicit TileMap(const char* file);

  void init(Graphic& graphic, World& world);
  [[nodiscard]] auto render_position(int x, int y) const -> RenderPosition;

 private:
  parser::TileMap data_;
  std::map<int, TileSet> tilesets_;
};

#endif
