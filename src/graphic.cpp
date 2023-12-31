#include "graphic.h"

#include "camera.h"

Graphic::Graphic(std::string&& title, int width, int height)  // NOLINT
    : title_{std::move(title)},
      width_{width},
      height_{height},
      window_{
          SDL_CreateWindow(title_.c_str(), width_, height_, SDL_WINDOW_OPENGL)},
      renderer_{SDL_CreateRenderer(
          window_, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)},
      camera_{width, height} {}

Graphic::~Graphic() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
};

auto Graphic::window() const -> SDL_Window* { return window_; }
auto Graphic::renderer() const -> SDL_Renderer* { return renderer_; }
auto Graphic::camera() -> Camera& { return camera_; }
auto Graphic::width() const -> int { return width_; }
auto Graphic::height() const -> int { return height_; }
