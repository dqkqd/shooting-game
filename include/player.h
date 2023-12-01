#ifndef PLAYER_H
#define PLAYER_H

#include "ecs/world.h"
#include "graphic.h"

namespace player {

struct IsPlayer {};

void init(Graphic& graphic, World& world);

}  // namespace player

#endif
