#ifndef PLAYER_SHOOTER_H
#define PLAYER_SHOOTER_H

#include "SDL_events.h"
#include "camera.h"
#include "ecs/world.h"

struct ShooterInfo {
  Point point;
};

class Shooter {
 public:
  static void shoot_system(World& world, SDL_Event event, Camera& camera);
  static void assign_position_system(World& world, SDL_Event event);
};

#endif
