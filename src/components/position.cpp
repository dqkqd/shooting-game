#include "components/position.h"

#include <algorithm>
#include <array>
#include <functional>

#include "SDL_rect.h"
#include "config.h"

[[nodiscard]] auto RenderPosition::top_left() const -> SDL_FPoint {
  return {.x = rect.x, .y = rect.y};
}
[[nodiscard]] auto RenderPosition::top_right() const -> SDL_FPoint {
  return {.x = rect.x + rect.w, .y = rect.y};
}
[[nodiscard]] auto RenderPosition::bot_left() const -> SDL_FPoint {
  return {.x = rect.x, .y = rect.y + rect.h};
}
[[nodiscard]] auto RenderPosition::bot_right() const -> SDL_FPoint {
  return {.x = rect.x + rect.w, .y = rect.y + rect.h};
}
[[nodiscard]] auto RenderPosition::points() const -> std::array<SDL_FPoint, 4> {
  return {top_left(), top_right(), bot_left(), bot_right()};
}

[[nodiscard]] auto RenderPosition::collide(const RenderPosition& position) const
    -> bool {
  auto edge_points = points();
  return std::any_of(edge_points.begin(), edge_points.end(),
                     [&position](SDL_FPoint& point) {
                       return SDL_PointInRectFloat(&point, &position.rect);
                     });
}

[[nodiscard]] auto RenderPosition::closest_offset(
    const RenderPosition& position, const Offset& target) -> Offset {
  auto closest_dx = find_closest_offset_one_direction(
      position, target.dx, [](auto current_position, auto value) {
        current_position.rect.x += value;
        return current_position;
      });

  auto closest_dy = find_closest_offset_one_direction(
      position, target.dy, [](auto current_position, auto value) {
        current_position.rect.y += value;
        return current_position;
      });

  return {.dx = closest_dx, .dy = closest_dy};
}

auto RenderPosition::find_closest_offset_one_direction(
    const RenderPosition& position, float good,
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
