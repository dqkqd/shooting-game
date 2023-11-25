#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <string>

#include "SDL_render.h"
#include "SDL_video.h"

class Graphic {
 public:
  explicit Graphic(std::string&& title, int width, int height)  // NOLINT
      : title_{std::move(title)},
        width_{width},
        height_{height},
        window_{SDL_CreateWindow(title_.c_str(), width_, height_,
                                 SDL_WINDOW_OPENGL)},
        renderer_{SDL_CreateRenderer(
            window_, NULL,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)} {
    ;
  }

  ~Graphic() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
  };

  Graphic(const Graphic&) = default;
  Graphic(Graphic&&) = default;
  auto operator=(const Graphic&) -> Graphic& = default;
  auto operator=(Graphic&&) -> Graphic& = default;

  [[nodiscard]] auto window() const -> SDL_Window* { return window_; }
  [[nodiscard]] auto renderer() const -> SDL_Renderer* { return renderer_; }

 private:
  std::string title_;
  int width_;
  int height_;

  SDL_Window* window_ = NULL;
  SDL_Renderer* renderer_ = NULL;
};
#endif
