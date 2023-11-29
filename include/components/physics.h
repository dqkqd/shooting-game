#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include "config.h"

struct Collidable {};

class Tick {
 public:
  auto next_tick() -> float {
    tick_ += TICK_OFFSET;
    return tick_;
  }

 private:
  float tick_;
};

class FreeFallMotion {
 public:
  auto vy() -> float {
    vy_ = vy_ + gravity_ * tick_.next_tick();
    return vy_;
  }

 private:
  float vy_{};
  Tick tick_{};
  float gravity_ = DEFAULT_GRAVITY / PHYSIC_SCALE;
};

#endif
