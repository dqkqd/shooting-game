#ifndef PLAYER_H
#define PLAYER_H

#include "components/primitive.h"
#include "ecs/world.h"
#include "graphic.h"

namespace player {

class NormalAnimation {
 public:
  auto next_sprite() -> int;
  auto position() -> TexturePosition;

 private:
  int current_sprite_ = 0;
};

void init(Graphic& graphic, World& world);

}  // namespace player

#endif
