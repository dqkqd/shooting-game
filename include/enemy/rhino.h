#ifndef ENEMY_RHINO_H
#define ENEMY_RHINO_H

#include "ecs/world.h"
#include "graphic.h"

struct RhinoInfo {
  float speed;
  Point from;
  Point to;
};

class Rhino {
 public:
  static void init(World& world, Graphic& graphic);

  static void moving_system(World& world);
};

#endif
