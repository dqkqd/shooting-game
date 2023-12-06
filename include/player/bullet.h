#ifndef PLAYER_BULLET_H
#define PLAYER_BULLET_H

#include "ecs/world.h"
#include "graphic.h"

enum class BulletStatus { HIDDEN, SHOOT };

struct BulletInfo {
  BulletStatus status = BulletStatus::HIDDEN;
};

class Bullet {
 public:
  static void init(World &World, Graphic &graphic);

  static void moving_system(World &world);
};

#endif
