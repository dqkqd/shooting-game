#ifndef GOAL_H
#define GOAL_H

#include "ecs/world.h"
#include "graphic.h"

struct GoalInfo {};

class Goal {
 public:
  static void init(World& world, Graphic& graphic);
};

#endif
