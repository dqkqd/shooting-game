#include "components/position.h"

#include <algorithm>
#include <array>

#include "SDL_rect.h"

auto RenderPosition::top_left() const -> SDL_FPoint {
  return {.x = rect.x, .y = rect.y};
}
auto RenderPosition::top_right() const -> SDL_FPoint {
  return {.x = rect.x + rect.w, .y = rect.y};
}
auto RenderPosition::bot_left() const -> SDL_FPoint {
  return {.x = rect.x, .y = rect.y + rect.h};
}
auto RenderPosition::bot_right() const -> SDL_FPoint {
  return {.x = rect.x + rect.w, .y = rect.y + rect.h};
}
auto RenderPosition::points() const -> std::array<SDL_FPoint, 4> {
  return {top_left(), top_right(), bot_left(), bot_right()};
}

auto RenderPosition::with_x(float x) const -> RenderPosition {
  return {x, rect.y, rect.w, rect.h};
}
auto RenderPosition::with_y(float y) const -> RenderPosition {
  return {rect.x, y, rect.w, rect.h};
}

auto RenderPosition::collide(const RenderPosition& position) const -> bool {
  auto edge_points = points();
  return std::any_of(edge_points.begin(), edge_points.end(),
                     [&position](SDL_FPoint& point) {
                       return SDL_PointInRectFloat(&point, &position.rect);
                     });
}
