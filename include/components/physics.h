#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include "components/position.h"
#include "config.h"

struct Collidable {};

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
  [[nodiscard]] auto movable() const -> bool;
  void reset();

 private:
  float vx_{};
  float vy_{};
  Tick tick_{};

  float g_ = DEFAULT_GRAVITY;
};
#endif
