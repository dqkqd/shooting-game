#include "components/texture.h"

#include <cassert>
#include <utility>

#include "SDL3_image/SDL_image.h"
#include "SDL_error.h"
#include "SDL_log.h"
#include "SDL_render.h"

std::unordered_map<const char*, SDL_Texture*> TextureManager::textures_{};

void TextureManager::clear() {
  for (auto [_, texture] : textures_) {
    SDL_DestroyTexture(texture);
  }
  textures_.clear();
}

auto TextureManager::add(const char* key, SDL_Texture* texture)
    -> SDL_Texture* {
  auto [it, inserted] = textures_.emplace(key, texture);
  if (!inserted) {
    // duplicated texture, remove the new one to avoid memory leaked
    SDL_DestroyTexture(texture);
    texture = NULL;
  }

  return it->second;
}

auto TextureManager::add_from_file(SDL_Renderer* renderer, const char* file)
    -> std::optional<SDL_Texture*> {
  auto it = textures_.find(file);
  if (it != textures_.end()) {
    return it->second;
  }

  auto* texture = IMG_LoadTexture(renderer, file);
  if (texture == NULL) {
    SDL_Log("Error loading texture: %s\n", IMG_GetError());
    return {};
  }

  return add(file, texture);
}

auto TextureManager::add_from_file_unchecked(SDL_Renderer* renderer,
                                             const char* file) -> SDL_Texture* {
  auto it = textures_.find(file);
  if (it != textures_.end()) {
    return it->second;
  }

  auto* texture = IMG_LoadTexture(renderer, file);
  assert(texture != NULL);
  return add(file, texture);
}

auto TextureManager::get(const char* key) -> std::optional<SDL_Texture*> {
  auto it = textures_.find(key);
  if (it == textures_.end()) {
    return {};
  }
  return it->second;
}

auto get_texture_size(SDL_Texture* texture) -> TextureSize {
  int w = 0;
  int h = 0;
  if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) < 0) {
    SDL_Log("`get_texture_size` failed: %s\n", SDL_GetError());
    return {};
  }
  return {.w = static_cast<float>(w), .h = static_cast<float>(h)};
}
