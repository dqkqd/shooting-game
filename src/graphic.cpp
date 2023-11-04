#include "graphic.h"

#include "SDL_error.h"
#include "SDL_init.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "SDL_video.h"

Graphic::Graphic(int width, int height, const char* title) {
  window_ = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
  if (window_ == NULL) {
    // TODO(khanhdq): throw error
  }
  SDL_SetWindowTitle(window_, title);
}

Graphic::~Graphic() { SDL_DestroyWindow(window_); }

auto Graphic::surface() -> SDL_Surface* {
  return SDL_GetWindowSurface(window_);
}

void Graphic::update_surface() { SDL_UpdateWindowSurface(window_); }