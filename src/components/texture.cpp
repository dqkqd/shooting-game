#include "components/texture.h"

#include <utility>

#include "SDL3_image/SDL_image.h"
#include "SDL_render.h"

Texture::Texture(Texture&& texture) noexcept
    : data_{std::exchange(texture.data_, nullptr)} {}
auto Texture::operator=(Texture&& texture) noexcept -> Texture& {
  data_ = std::exchange(texture.data_, nullptr);
  return *this;
}
Texture::~Texture() { SDL_DestroyTexture(data_); }

Texture::Texture(SDL_Texture* data) : data_{std::exchange(data, nullptr)} {}

auto Texture::data() const -> SDL_Texture* { return data_; }

auto Texture::from_file(SDL_Renderer* renderer, const char* file)
    -> std::optional<Texture> {
  auto* texture = IMG_LoadTexture(renderer, file);
  if (texture == NULL) {
    SDL_Log("Error loading image: %s\n", IMG_GetError());
    return {};
  }
  return Texture{texture};
}
