#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "ecs/world.h"
#include "graphic.h"

namespace player {

enum class Status {
  IDLE,
  STOPPED,
  SELF_SHOOTING,
};

struct Player {
  Status status = Status::IDLE;
};

struct ShootPosition {
  float x{};
  float y{};
};

void init(Graphic& graphic, World& world);

void animation_system(World& world);
void moving_system(World& world);
void camera_system(World& world, Camera& camera);

void shoot_system(World& world, SDL_Event event, Camera& camera);
void assign_shoot_position(World& world, SDL_Event event);

}  // namespace player

#endif
