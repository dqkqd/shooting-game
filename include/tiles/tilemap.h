#ifndef TILES_TILESET_H
#define TILES_TILESET_H

#include <cstddef>
#include <utility>

#include "SDL_render.h"
#include "components/primitive.h"
#include "components/texture.h"
#include "ecs/world.h"
#include "graphic.h"
#include "tiles/parser.h"

class TileSet {
 public:
  explicit TileSet(parser::TileSetInMap data) : data_{std::move(data)} {};

  void init(Graphic& graphic) {
    auto source = CONFIG_FOLDER + data_.tile_set.image;
    texture_ = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                       source.c_str());
  }

  auto texture() -> SDL_Texture* { return texture_; }
  auto data() -> parser::TileSetInMap { return data_; }

  [[nodiscard]] auto position(int index) const -> Position {
    auto w = data_.tile_set.tile_width;
    auto h = data_.tile_set.tile_height;
    auto x = index % data_.tile_set.columns * w;
    auto y = index / data_.tile_set.columns * h;
    return Position{static_cast<float>(x), static_cast<float>(y),
                    static_cast<float>(w), static_cast<float>(h)};
  };

 private:
  parser::TileSetInMap data_;
  SDL_Texture* texture_ = NULL;
};

struct TilePosition {
  Position src;
  Position dest;
};

class TileMap {
 public:
  explicit TileMap(const char* file) {
    std::ifstream f(file);
    json parsed = json::parse(f);
    data_ = parsed.template get<parser::TileMap>();

    for (auto tileset : data_.tile_sets) {
      tilesets_.emplace(tileset.gid, tileset);
    }
  }

  void init(Graphic& graphic, World& world) {
    for (auto& [_, tileset] : tilesets_) {
      tileset.init(graphic);
    }

    for (int y = 0; y < data_.height; ++y) {
      for (int x = 0; x < data_.width; ++x) {
        auto pos = y * data_.width + x;
        auto tile_id = data_.data[pos];
        auto it = std::prev(tilesets_.upper_bound(tile_id));

        auto src_position =
            it->second.position(tile_id - it->second.data().gid);
        auto dest_position = position(x, y);

        world.spawn_entity_with<SDL_Texture*, TilePosition>(
            std::move(it->second.texture()),
            {.src = src_position, .dest = dest_position});
      }
    }

    std::function<void(Query<SDL_Texture*, TilePosition>)> tile_render_system =
        [&graphic](Query<SDL_Texture*, TilePosition> query) {
          for (auto [query_texture, position] : query) {
            SDL_RenderTexture(graphic.renderer(), query_texture,
                              &position.src.rect, &position.dest.rect);
          }
        };

    world.add_system(std::move(tile_render_system));
  }

  [[nodiscard]] auto position(int x, int y) const -> Position {
    auto w = static_cast<float>(data_.tile_width);
    auto h = static_cast<float>(data_.tile_height);
    return Position{static_cast<float>(x) * w, static_cast<float>(y) * h, w, h};
  };

 private:
  parser::TileMap data_;
  std::map<int, TileSet> tilesets_;
};

#endif
