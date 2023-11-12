#include "graphic.h"

#include <error.h>

#include <cstddef>

#include "SDL3_image/SDL_image.h"
#include "SDL_error.h"
#include "SDL_init.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include "components/graphic.h"

Graphic::Graphic(int width, int height, const char* title)
    : width_{width}, height_{height} {
  if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &window_,
                                  &renderer_) < 0) {
    throw GameError::from_sdl("Can not create window and renderer for graphic");
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

void Graphic::render(GraphicComponent& component) {
  SDL_RenderTexture(renderer_, component.get_texture(), NULL,
                    &component.get_position());
}
void Graphic::update() { SDL_RenderPresent(renderer_); }
