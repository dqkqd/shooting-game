#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <algorithm>
#include <array>

#include "SDL_rect.h"

struct Position {
  SDL_FRect rect;
};

struct TexturePosition {
  SDL_FRect rect;
};

struct RenderPosition {
  SDL_FRect rect;

  [[nodiscard]] auto top_left() const -> SDL_FPoint {
    return {.x = rect.x, .y = rect.y};
  }
  [[nodiscard]] auto top_right() const -> SDL_FPoint {
    return {.x = rect.x + rect.w, .y = rect.y};
  }
  [[nodiscard]] auto bot_left() const -> SDL_FPoint {
    return {.x = rect.x, .y = rect.y + rect.h};
  }
  [[nodiscard]] auto bot_right() const -> SDL_FPoint {
    return {.x = rect.x + rect.w, .y = rect.y + rect.h};
  }
  [[nodiscard]] auto points() const -> std::array<SDL_FPoint, 4> {
    return {top_left(), top_right(), bot_left(), bot_right()};
  }

  [[nodiscard]] auto collide(const RenderPosition& position) const -> bool {
    auto edge_points = points();
    return std::any_of(edge_points.begin(), edge_points.end(),
                       [&position](SDL_FPoint& point) {
                         return SDL_PointInRectFloat(&point, &position.rect);
                       });
  }
};

struct Speed {
  float vx;
  float vy;
};

#endif
