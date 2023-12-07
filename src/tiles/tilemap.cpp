#include "tiles/tilemap.h"

#include <algorithm>

#include "components/physics.h"
#include "config.h"
#include "services/texture.h"

TileSet::TileSet(parser::TileSetInMap data) : data_{std::move(data)} {};

void TileSet::init(Graphic& graphic) {
  auto source =
      GameConfig::data().assets.config_folder + '/' + data_.tile_set.image;
  texture_ = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                     source.c_str());
}

auto TileSet::texture() -> SDL_Texture* { return texture_; }
auto TileSet::data() -> parser::TileSetInMap { return data_; }

auto TileSet::collidable(int tile_id) const -> bool {
  auto& collidables = GameConfig::data().tile_map.collidables;
  return std::any_of(collidables.cbegin(), collidables.cend(),
                     [this, &tile_id](auto collidable) {
                       return data_.gid == collidable.gid &&
                              collidable.ids.count(tile_id - data_.gid) > 0;
                     });
}

auto TileSet::texture_info(int index) const -> TextureInfo {
  auto w = data_.tile_set.tile_width;
  auto h = data_.tile_set.tile_height;
  auto x = index % data_.tile_set.columns * w;
  auto y = index / data_.tile_set.columns * h;
  return TextureInfo{{static_cast<float>(x), static_cast<float>(y),
                      static_cast<float>(w), static_cast<float>(h)}};
};

TileMap::TileMap(const char* file) {
  std::ifstream f(file);
  json parsed = json::parse(f);
  data_ = parsed.template get<parser::TileMap>();

  for (auto tileset : data_.tile_sets) {
    tilesets_.emplace(tileset.gid, tileset);
  }
}

void TileMap::init(World& world, Graphic& graphic) {
  for (auto& [_, tileset] : tilesets_) {
    tileset.init(graphic);
  }

  for (int y = 0; y < data_.height; ++y) {
    for (int x = 0; x < data_.width; ++x) {
      auto pos = y * data_.width + x;
      auto tile_id = data_.data[pos];
      auto it = std::prev(tilesets_.upper_bound(tile_id));

      auto src = it->second.texture_info(tile_id - it->second.data().gid);
      auto dest = render_info(x, y);

      if (it->second.collidable(tile_id)) {
        world.spawn_entity_with(std::move(it->second.texture()), std::move(src),
                                std::move(dest), Collidable{});
      } else {
        world.spawn_entity_with(std::move(it->second.texture()), std::move(src),
                                std::move(dest));
      }
    }
  }
}

auto TileMap::render_info(int x, int y) const -> RenderInfo {
  auto w = static_cast<float>(data_.tile_width);
  auto h = static_cast<float>(data_.tile_height);
  return RenderInfo{static_cast<float>(x) * w, static_cast<float>(y) * h, w, h};
};
