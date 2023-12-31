#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <string>

#include "SDL_render.h"
#include "SDL_video.h"
#include "camera.h"

class Graphic {
 public:
  explicit Graphic(std::string&& title, int width, int height);  // NOLINT

  ~Graphic();

  Graphic(const Graphic&) = default;
  Graphic(Graphic&&) = default;
  auto operator=(const Graphic&) -> Graphic& = default;
  auto operator=(Graphic&&) -> Graphic& = default;

  [[nodiscard]] auto window() const -> SDL_Window*;
  [[nodiscard]] auto renderer() const -> SDL_Renderer*;
  [[nodiscard]] auto camera() -> Camera&;
  [[nodiscard]] auto width() const -> int;
  [[nodiscard]] auto height() const -> int;

 private:
  std::string title_;
  int width_;
  int height_;

  SDL_Window* window_ = NULL;
  SDL_Renderer* renderer_ = NULL;

  Camera camera_;
};
#endif
