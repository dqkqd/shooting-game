#include "components/position.h"

#include <algorithm>
#include <array>

#include "SDL_rect.h"

auto RenderInfo::center() const -> SDL_FPoint {
  return {.x = rect.x + rect.w / 2, .y = rect.y + rect.h / 2};
}

auto RenderInfo::top_left() const -> SDL_FPoint {
  return {.x = rect.x, .y = rect.y};
}
auto RenderInfo::top_right() const -> SDL_FPoint {
  return {.x = rect.x + rect.w, .y = rect.y};
}
auto RenderInfo::bot_left() const -> SDL_FPoint {
  return {.x = rect.x, .y = rect.y + rect.h};
}
auto RenderInfo::bot_right() const -> SDL_FPoint {
  return {.x = rect.x + rect.w, .y = rect.y + rect.h};
}
auto RenderInfo::points() const -> std::array<SDL_FPoint, 4> {
  return {top_left(), top_right(), bot_left(), bot_right()};
}

auto RenderInfo::with_x(float x) const -> RenderInfo {
  return {x, rect.y, rect.w, rect.h};
}
auto RenderInfo::with_y(float y) const -> RenderInfo {
  return {rect.x, y, rect.w, rect.h};
}

auto RenderInfo::collide(const RenderInfo& info) const -> bool {
  auto edge_points = points();
  return std::any_of(edge_points.begin(), edge_points.end(),
                     [&info](SDL_FPoint& point) {
                       return SDL_PointInRectFloat(&point, &info.rect);
                     });
}
