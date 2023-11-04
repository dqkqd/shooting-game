#include "graphic.h"

#include <cstddef>

#include "SDL3_image/SDL_image.h"
#include "SDL_error.h"
#include "SDL_init.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "SDL_video.h"

Graphic::Graphic(int width, int height, const char* title)
    : width_{width}, height_{height} {
  if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &window_,
                                  &renderer_) < 0) {
    // TODO(khanhdq): throw error
  }
  SDL_SetWindowTitle(window_, title);
}

Graphic::~Graphic() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

auto Graphic::load_texture(const char* filename) -> SDL_Texture* {
  return IMG_LoadTexture(renderer_, filename);
}

void Graphic::update() { SDL_RenderPresent(renderer_); }

auto Graphic::renderer() -> SDL_Renderer* { return renderer_; }