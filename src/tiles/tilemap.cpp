#include "tiles/tilemap.h"

#include "components/texture.h"

TileSet::TileSet(parser::TileSetInMap data) : data_{std::move(data)} {};

void TileSet::init(Graphic& graphic) {
  auto source = ASSETS_CONFIG_FOLDER + data_.tile_set.image;
  texture_ = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                     source.c_str());
}

auto TileSet::texture() -> SDL_Texture* { return texture_; }
auto TileSet::data() -> parser::TileSetInMap { return data_; }

auto TileSet::position(int index) const -> Position {
  auto w = data_.tile_set.tile_width;
  auto h = data_.tile_set.tile_height;
  auto x = index % data_.tile_set.columns * w;
  auto y = index / data_.tile_set.columns * h;
  return Position{static_cast<float>(x), static_cast<float>(y),
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

  std::vector<std::tuple<SDL_Texture*, TilePosition>> textures;

  for (int y = 0; y < data_.height; ++y) {
    for (int x = 0; x < data_.width; ++x) {
      auto pos = y * data_.width + x;
      auto tile_id = data_.data[pos];
      auto it = std::prev(tilesets_.upper_bound(tile_id));

      auto src_position = it->second.position(tile_id - it->second.data().gid);
      auto dest_position = position(x, y);

      auto texture = std::make_tuple(
          it->second.texture(),
          TilePosition{.src = src_position, .dest = dest_position});
      textures.emplace_back(texture);
    }
  }

  std::function<void()> tile_render_system = [textures, &graphic]() {
    for (auto [query_texture, position] : textures) {
      SDL_RenderTexture(graphic.renderer(), query_texture, &position.src.rect,
                        &position.dest.rect);
    }
  };

  world.add_system(std::move(tile_render_system));
}

auto TileMap::position(int x, int y) const -> Position {
  auto w = static_cast<float>(data_.tile_width);
  auto h = static_cast<float>(data_.tile_height);
  return Position{static_cast<float>(x) * w, static_cast<float>(y) * h, w, h};
};
