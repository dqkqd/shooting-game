#ifndef TILES_PARSER_H
#define TILES_PARSER_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

constexpr char const* ASSETS_FOLDER = "assets";
constexpr char const* CONFIG_FOLDER = "assets/config/";

namespace parser {

struct TileSet {
  int columns;
  std::string image;
  int tile_width;
  int tile_height;
};

struct TileSetInMap {
  int gid{};
  std::string source;
  TileSet tile_set;
};
struct TileMap {
  int width;
  int height;
  int tile_width;
  int tile_height;
  std::vector<int> data;

  std::vector<TileSetInMap> tile_sets{};
};

inline void from_json(const json& j, TileSet& tile_set) {
  j.at("columns").get_to(tile_set.columns);
  j.at("image").get_to(tile_set.image);
  j.at("tilewidth").get_to(tile_set.tile_width);
  j.at("tileheight").get_to(tile_set.tile_height);
}

inline void from_json(const json& j, TileSetInMap& tile_set_in_map) {
  j.at("firstgid").get_to(tile_set_in_map.gid);
  j.at("source").get_to(tile_set_in_map.source);

  std::ifstream f(CONFIG_FOLDER + tile_set_in_map.source);
  json data = json::parse(f);
  tile_set_in_map.tile_set = data.template get<TileSet>();
}

inline void from_json(const json& j, TileMap& tile_map) {
  j.at("width").get_to(tile_map.width);
  j.at("height").get_to(tile_map.height);
  j.at("tilewidth").get_to(tile_map.tile_width);
  j.at("tileheight").get_to(tile_map.tile_height);

  // we only have 1 layer
  for (const auto& layer : j["layers"]) {
    layer.at("data").get_to(tile_map.data);
  }

  for (const auto& tileset : j["tilesets"]) {
    tile_map.tile_sets.push_back(tileset.template get<TileSetInMap>());
  }
}
};  // namespace parser

#endif
