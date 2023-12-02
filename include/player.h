#ifndef PLAYER_H
#define PLAYER_H

#include "SDL_log.h"
#include "camera.h"
#include "components/animation.h"
#include "ecs/world.h"
#include "graphic.h"

namespace player {

struct IsPlayer {};

void init(Graphic& graphic, World& world);

void animation_system(World& world);
void moving_system(World& world);
void camera_system(World& world, Camera& camera);

inline void test_player_event(World& world, SDL_Event event) {
  SDL_Log("Input: %d %d", event.key.keysym.scancode, event.key.keysym.sym);
}

}  // namespace player

#endif
