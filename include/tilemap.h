#ifndef TILEMAP_H
#define TILEMAP_H

#include <array>

#include "SDL_render.h"
#include "components/primitive.h"
#include "components/texture.h"
#include "ecs/query/query.h"
#include "ecs/world.h"
#include "graphic.h"

constexpr int INVALID_INDEX = 0;

const std::array<std::array<int, 20>, 15> MAP{{
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3},
    {4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5},
    {4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5},
}};

namespace tile_filename {

constexpr char const *TILE_SET = "assets/tileset.png";

};  // namespace tile_filename

struct TilePosition {
  Position src;
  Position dest;
};

class TileMap {
 public:
  static void init(const Graphic &graphic, World &world) {
    auto *texture = TextureManager::add_from_file_unchecked(
        graphic.renderer(), tile_filename::TILE_SET);

    for (int y = 0; y < static_cast<int>(MAP.size()); ++y) {
      for (int x = 0; x < static_cast<int>(MAP[y].size()); ++x) {
        auto index = MAP[y][x];
        if (index == INVALID_INDEX) {
          continue;
        }
        world.spawn_entity_with<SDL_Texture *, TilePosition>(
            std::move(texture),
            {.src = src_position(index), .dest = dest_position(x, y)});
      }
    }

    std::function<void(Query<SDL_Texture *, TilePosition>)> tile_render_system =
        [&graphic](Query<SDL_Texture *, TilePosition> query) {
          for (auto [query_texture, position] : query) {
            SDL_RenderTexture(graphic.renderer(), query_texture,
                              &position.src.rect, &position.dest.rect);
          }
        };

    world.add_system(std::move(tile_render_system));
  }

 private:
  static constexpr int TILE_SIZE = 32;
  static constexpr int TILE_ROWS = 2;
  static constexpr int TILE_COLUMNS = 6;

  static auto position(int x, int y) -> Position {
    return {static_cast<float>(x) * TILE_SIZE,
            static_cast<float>(y) * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  }

  static auto dest_position(int r, int c) -> Position { return position(r, c); }
  static auto src_position(int index) -> Position {
    int x = (index - 1) % TILE_COLUMNS;
    int y = (index - 1) / TILE_COLUMNS;
    return position(x, y);
  }
};

#endif
