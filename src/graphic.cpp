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
  for (auto* texture : textures_) {
    SDL_DestroyTexture(texture);
  }
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Graphic::add_texture_from_file(const char* filename) {
  textures_.push_back(IMG_LoadTexture(renderer_, filename));
}

void Graphic::render() {
  for (auto* texture : textures_) {
    // TODO(khanhdq): this should be created when add_texture
    int texture_width = 0;
    int texture_height = 0;

    SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

    SDL_FRect dest_rect{0, static_cast<float>(height_ - texture_height),
                        static_cast<float>(texture_width),
                        static_cast<float>(texture_height)};

    SDL_RenderTexture(renderer_, texture, NULL, &dest_rect);
  }
  SDL_RenderPresent(renderer_);
}