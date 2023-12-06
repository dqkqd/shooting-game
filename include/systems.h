#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs/world.h"
#include "graphic.h"

namespace shared_systems {

void render_system(World& world, Graphic& graphic);
void animation_system(World& world);

};  // namespace shared_systems

#endif
