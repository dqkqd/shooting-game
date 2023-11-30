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
                                   const float target_y) const -> float {
    /* Find the best offset which will not result in collision using binary
     * search */
    float bad_y = 0;
    float good_y = target_y;

    while (std::abs(good_y - bad_y) > PIXEL_FLOAT_OFFSET) {
      float y = (bad_y + good_y) / 2;
      auto next_position = *this;
      next_position.rect.y += y;

      if (next_position.collide(position)) {
        good_y = y;
      } else {
        bad_y = y;
      }
    }
    return bad_y;
  };

  [[nodiscard]] auto best_x_offset(const RenderPosition& position,
                                   const float target_x) const -> float {
    /* Find the best offset which will not result in collision using binary
     * search */
    float bad_x = 0;
    float good_x = target_x;
    while (std::abs(good_x - bad_x) > PIXEL_FLOAT_OFFSET) {
      float x = (bad_x + good_x) / 2;
      auto next_position = *this;
      next_position.rect.x += x;

      if (next_position.collide(position)) {
        good_x = x;
      } else {
        bad_x = x;
      }
    }
    return bad_x;
  };
};

#endif
