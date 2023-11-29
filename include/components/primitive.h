#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <algorithm>
#include <array>

#include "SDL_rect.h"
#include "config.h"

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

  [[nodiscard]] auto best_y_offset(const RenderPosition& position,
                                   const float max_y) const -> float {
    /* Find the best offset which will not result in collision using binary
     * search */
    float low_y = 0;
    float high_y = max_y;
    while (low_y + PIXEL_FLOAT_OFFSET < high_y) {
      float y = (low_y + high_y) / 2;
      auto next_position = *this;
      next_position.rect.y += y;

      if (next_position.collide(position)) {
        high_y = y;
      } else {
        low_y = y;
      }
    }
    return low_y;
  };

  [[nodiscard]] auto best_x_offset(const RenderPosition& position,
                                   const float max_x) const -> float {
    /* Find the best offset which will not result in collision using binary
     * search */
    float low_x = 0;
    float high_x = max_x;
    while (low_x + PIXEL_FLOAT_OFFSET < high_x) {
      float x = (low_x + high_x) / 2;
      auto next_position = *this;
      next_position.rect.y += x;

      if (next_position.collide(position)) {
        high_x = x;
      } else {
        low_x = x;
      }
    }
    return low_x;
  };
};

struct Speed {
  float vx;
  float vy;
};

#endif
