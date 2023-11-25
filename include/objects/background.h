#ifndef OBJECTS_BACKGROUND_H
#define OBJECTS_BACKGROUND_H

#include "SDL_render.h"
#include "ecs/world.h"

class BackGround {
 public:
  explicit BackGround(SDL_Renderer* renderer);

  void setup(World& world);

 private:
  SDL_Texture* top_left_grass_;
  SDL_Texture* top_right_grass_;
  SDL_Texture* top_middle_grass_;
};

#endif
