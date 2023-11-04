#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <vector>

#include "SDL_render.h"
#include "SDL_video.h"

class GraphicComponent;

class Graphic {
 public:
  Graphic(int width, int height, const char* title);
  Graphic(const Graphic&) = delete;
  Graphic(Graphic&&) = delete;
  auto operator=(const Graphic&) -> Graphic& = delete;
  auto operator=(Graphic&&) -> Graphic& = delete;
  ~Graphic();

  auto load_texture(const char* filename) -> SDL_Texture*;

  void render(GraphicComponent& component);
  void update();

 private:
  int width_;
  int height_;
  SDL_Window* window_ = NULL;
  SDL_Renderer* renderer_ = NULL;
};

#endif
