#ifndef POSITION_H
#define POSITION_H

#include <algorithm>
#include <array>
#include <functional>

#include "SDL_rect.h"
#include "config.h"

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
    return best_offset(position, target_y,
                       [](auto current_position, auto value) {
                         current_position.rect.y += value;
                         return current_position;
                       });
  };

  [[nodiscard]] auto best_x_offset(const RenderPosition& position,
                                   const float target_x) const -> float {
    return best_offset(position, target_x,
                       [](auto current_position, auto value) {
                         current_position.rect.x += value;
                         return current_position;
                       });
  };

 private:
  auto best_offset(
      const RenderPosition& position, const float good,
      const std::function<RenderPosition(const RenderPosition&, float value)>&
          next_position_func) const -> float {
    /* Find the best offset which will not result in collision using binary
     * search */
    float start = 0;
    float end = good;
    while (std::abs(start - end) > PIXEL_FLOAT_OFFSET) {
      float mid = (start + end) / 2;
      auto next_position = next_position_func(*this, mid);
      if (next_position.collide(position)) {
        end = mid;
      } else {
        start = mid;
      }
    }

    return start;
  }
};

#endif
