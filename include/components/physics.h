#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include "config.h"

class Falling {
 public:
  auto vy() -> float {
    vy_ = vy_ + gravity_ * ticks_;
    ticks_ += TICK_OFFSET;
    return vy_;
  }

 private:
  float vy_{};
  float ticks_{};
  float gravity_ = DEFAULT_GRAVITY / PHYSIC_SCALE;
};

#endif
