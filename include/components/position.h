#ifndef COMPONENTS_POSITION_H
#define COMPONENTS_POSITION_H

#include <array>

#include "SDL_rect.h"

struct Offset {
  float dx;
  float dy;
};

struct TexturePosition {
  SDL_FRect rect;
};

struct RenderPosition {
  SDL_FRect rect;

  [[nodiscard]] auto top_left() const -> SDL_FPoint;
  [[nodiscard]] auto top_right() const -> SDL_FPoint;
  [[nodiscard]] auto bot_left() const -> SDL_FPoint;
  [[nodiscard]] auto bot_right() const -> SDL_FPoint;
  [[nodiscard]] auto points() const -> std::array<SDL_FPoint, 4>;

  [[nodiscard]] auto with_x(float x) const -> RenderPosition;
  [[nodiscard]] auto with_y(float y) const -> RenderPosition;

  [[nodiscard]] auto collide(const RenderPosition& position) const -> bool;
};

#endif
