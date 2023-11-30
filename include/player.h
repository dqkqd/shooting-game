#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "components/position.h"
#include "ecs/world.h"
#include "graphic.h"

namespace player {

struct IsPlayer {};

class NormalAnimation {
 public:
  auto next_sprite() -> int;
  auto position() -> TexturePosition;

 private:
  int current_sprite_ = 0;
};

void init(Graphic& graphic, World& world, Camera& camera);

}  // namespace player

#endif
