#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "SDL_rect.h"
#include "SDL_render.h"

struct RenderComponent {
  SDL_Texture* texture;
  SDL_FRect position;
};

#endif
