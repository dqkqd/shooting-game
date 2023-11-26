#ifndef OBJECTS_BACKGROUND_H
#define OBJECTS_BACKGROUND_H

#include "ecs/world.h"
#include "graphic.h"

class BackGround {
 public:
  void init(const Graphic& graphic, World& world);

 private:
  SDL_Texture* top_left_grass_ = NULL;
  SDL_Texture* top_right_grass_ = NULL;
  SDL_Texture* top_middle_grass_ = NULL;
};

#endif
