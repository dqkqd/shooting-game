#ifndef PLAYER_SHOOTER_H
#define PLAYER_SHOOTER_H

#include "SDL_events.h"
#include "camera.h"
#include "ecs/world.h"
#include "graphic.h"

struct ShooterInfo {
  SDL_Texture* indicator_texture = NULL;
  TexturePosition src_position;
  std::array<SDL_FPoint, 50> points{};
  bool hidden = false;
};

class Shooter {
 public:
  static void init(World& world, Graphic& graphic);

  static void shoot_system(World& world, SDL_Event event, Camera& camera);
  static void assign_position_system(World& world, SDL_Event event,
                                     Camera& camera);
  static void indicator_render_system(World& world, Graphic& graphic);
};

#endif
