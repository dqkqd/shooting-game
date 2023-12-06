#ifndef COMPONENTS_POSITION_H
#define COMPONENTS_POSITION_H

#include <array>

#include "SDL_rect.h"
#include "SDL_render.h"

struct Offset {
  float dx;
  float dy;
};

struct TextureInfo {
  SDL_FRect rect{};
  SDL_RendererFlip flip = SDL_FLIP_NONE;
  bool hidden = false;
};

struct RenderInfo {
  SDL_FRect rect;

  [[nodiscard]] auto center() const -> SDL_FPoint;
  [[nodiscard]] auto top_left() const -> SDL_FPoint;
  [[nodiscard]] auto top_right() const -> SDL_FPoint;
  [[nodiscard]] auto bot_left() const -> SDL_FPoint;
  [[nodiscard]] auto bot_right() const -> SDL_FPoint;
  [[nodiscard]] auto points() const -> std::array<SDL_FPoint, 4>;

  [[nodiscard]] auto with_x(float x) const -> RenderInfo;
  [[nodiscard]] auto with_y(float y) const -> RenderInfo;

  [[nodiscard]] auto collide(const RenderInfo& info) const -> bool;
};

#endif
