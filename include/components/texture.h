#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <optional>

#include "SDL_render.h"

struct Texture {
  Texture(const Texture&) = delete;
  auto operator=(const Texture&) -> Texture& = delete;
  Texture(Texture&&) noexcept;
  auto operator=(Texture&&) noexcept -> Texture&;

  ~Texture();

  static auto from_file(SDL_Renderer* renderer, const char* file)
      -> std::optional<Texture>;

  [[nodiscard]] auto data() const -> SDL_Texture*;

 private:
  explicit Texture(SDL_Texture* data);
  SDL_Texture* data_;
};

#endif
