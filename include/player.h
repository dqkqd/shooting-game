#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "components/animation.h"
#include "ecs/world.h"
#include "graphic.h"

namespace player {

struct IsPlayer {};

void init(Graphic& graphic, World& world);

void animation_system(Query<TexturePosition, TextureAnimation> query);
void moving_system(World& world);
void camera_system(World& world, Camera& camera);

}  // namespace player

#endif
