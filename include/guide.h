#ifndef GUIDE_H
#define GUIDE_H

#include "ecs/world.h"
#include "graphic.h"

class Guide {
 public:
  static void init(World& world, Graphic& graphic);
};

#endif
