#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include "config.h"

struct Collidable {};

struct Offset {
  float dx;
  float dy;
};

class Tick {
 public:
  auto next_tick() -> float;
  [[nodiscard]] static auto dt() -> float;

 private:
  float tick_{};
};

class ProjectileMotion {
 public:
  ProjectileMotion(float initial_velocity, float alpha);  // NOLINT

  auto next_offset() -> Offset;

 private:
  float vx_{};
  float vy_{};
  Tick tick_{};

  float g_ = DEFAULT_GRAVITY;
};
#endif
