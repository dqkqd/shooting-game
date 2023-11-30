#ifndef COMPONENTS_POSITION_H
#define COMPONENTS_POSITION_H

#include <array>
#include <functional>

#include "SDL_rect.h"

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

  [[nodiscard]] auto collide(const RenderPosition& position) const -> bool;

  [[nodiscard]] auto best_y_offset(const RenderPosition& position,
                                   float target_y) const -> float;

  [[nodiscard]] auto best_x_offset(const RenderPosition& position,
                                   float target_x) const -> float;

 private:
  auto best_offset(
      const RenderPosition& position, float good,
      const std::function<RenderPosition(const RenderPosition&, float value)>&
          next_position_func) const -> float;
};

#endif
