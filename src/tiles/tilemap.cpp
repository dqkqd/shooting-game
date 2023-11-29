#include "tiles/tilemap.h"

#include "components/physics.h"
#include "components/primitive.h"
#include "config.h"
#include "services/texture.h"

TileSet::TileSet(parser::TileSetInMap data) : data_{std::move(data)} {};

void TileSet::init(Graphic& graphic) {
  auto source = ASSETS_CONFIG_FOLDER + data_.tile_set.image;
  texture_ = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                     source.c_str());
}

auto TileSet::texture() -> SDL_Texture* { return texture_; }
auto TileSet::data() -> parser::TileSetInMap { return data_; }

auto TileSet::collidable() const -> bool {
  return COLLIDABLE_TILESETS.count(data_.tile_set.name) > 0;
}

auto TileSet::texture_position(int index) const -> TexturePosition {
  auto w = data_.tile_set.tile_width;
  auto h = data_.tile_set.tile_height;
  auto x = index % data_.tile_set.columns * w;
  auto y = index / data_.tile_set.columns * h;
  return TexturePosition{static_cast<float>(x), static_cast<float>(y),
                         static_cast<float>(w), static_cast<float>(h)};
};

TileMap::TileMap(const char* file) {
  std::ifstream f(file);
  json parsed = json::parse(f);
  data_ = parsed.template get<parser::TileMap>();

  for (auto tileset : data_.tile_sets) {
    tilesets_.emplace(tileset.gid, tileset);
  }
}

void TileMap::init(Graphic& graphic, World& world) {
  for (auto& [_, tileset] : tilesets_) {
    tileset.init(graphic);
  }

  for (int y = 0; y < data_.height; ++y) {
    for (int x = 0; x < data_.width; ++x) {
      auto pos = y * data_.width + x;
      auto tile_id = data_.data[pos];
      auto it = std::prev(tilesets_.upper_bound(tile_id));

      auto src_position =
          it->second.texture_position(tile_id - it->second.data().gid);
      auto dest_position = render_position(x, y);

      if (it->second.collidable()) {
        world.spawn_entity_with(std::move(it->second.texture()),
                                std::move(src_position),
                                std::move(dest_position), Collidable{});
      } else {
        world.spawn_entity_with(std::move(it->second.texture()),
                                std::move(src_position),
                                std::move(dest_position));
      }
    }
  }

  std::function<void(Query<SDL_Texture*, TexturePosition, RenderPosition>)>
      system = [&graphic](
                   Query<SDL_Texture*, TexturePosition, RenderPosition> query) {
        for (auto [texture, src, dest] : query) {
          SDL_RenderTexture(graphic.renderer(), texture, &src.rect, &dest.rect);
        }
      };

  world.add_system(std::move(system));
}

auto TileMap::render_position(int x, int y) const -> RenderPosition {
  auto w = static_cast<float>(data_.tile_width);
  auto h = static_cast<float>(data_.tile_height);
  return RenderPosition{static_cast<float>(x) * w, static_cast<float>(y) * h, w,
                        h};
};
