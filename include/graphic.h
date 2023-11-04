#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <vector>

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

  void add_texture_from_file(const char* filename);
  void render();

 private:
  int width_;
  int height_;
  SDL_Window* window_ = NULL;
  SDL_Renderer* renderer_ = NULL;

  // TODO(khanhdq): use array, apply free list data structure
  std::vector<SDL_Texture*> textures_;
};

#endif