#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "SDL_render.h"
#include "SDL_video.h"

class Graphic {
 public:
  Graphic(int width, int height, const char* title);
  Graphic(const Graphic&) = delete;
  Graphic(Graphic&&) = delete;
  auto operator=(const Graphic&) -> Graphic& = delete;
  auto operator=(Graphic&&) -> Graphic& = delete;
  ~Graphic();

  auto surface() -> SDL_Surface*;
  void update_surface();

 private:
  SDL_Window* window_ = NULL;
};

#endif