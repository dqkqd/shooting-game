#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <optional>
#include <unordered_map>

#include "SDL_render.h"

namespace texture_file {

constexpr char const* TOP_MIDDLE_GRASS = "assets/top-middle-grass.png";
constexpr char const* GROUND = "assets/ground.png";

};  // namespace texture_file

class TextureManager {
 public:
  static void clear();

  static auto add(const char* key, SDL_Texture* texture) -> SDL_Texture*;
  static auto add_from_file(SDL_Renderer* renderer, const char* file)
      -> std::optional<SDL_Texture*>;

  static auto get(const char* key) -> std::optional<SDL_Texture*>;

 private:
  static std::unordered_map<const char*, SDL_Texture*> textures_;
};

#endif
