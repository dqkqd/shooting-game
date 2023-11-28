#ifndef SERVICE_TEXTURE_H
#define SERVICE_TEXTURE_H

#include <optional>
#include <unordered_map>

#include "SDL_render.h"

class TextureManager {
 public:
  static auto size() -> std::size_t;
  static void clear();

  static auto add(const char* key, SDL_Texture* texture) -> SDL_Texture*;
  static auto add_from_file(SDL_Renderer* renderer, const char* file)
      -> std::optional<SDL_Texture*>;
  static auto add_from_file_unchecked(SDL_Renderer* renderer, const char* file)
      -> SDL_Texture*;

  static auto get(const char* key) -> std::optional<SDL_Texture*>;

 private:
  static std::unordered_map<const char*, SDL_Texture*> textures_;
};

/* helper functions */
struct TextureSize {
  float w;
  float h;
};

auto get_texture_size(SDL_Texture* texture) -> TextureSize;

#endif
