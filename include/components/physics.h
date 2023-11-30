#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include <cmath>

#include "config.h"

struct Collidable {};

struct Offset {
  float dx;
  float dy;
};

class Tick {
 public:
  auto next_tick() -> float {
    tick_ += dt();
    return tick_;
  }

  [[nodiscard]] static auto dt() -> float { return TICK_OFFSET; }

 private:
  float tick_{};
};

class ProjectileMotion {
 public:
  ProjectileMotion(float initial_velocity, float alpha)  // NOLINT
      : vx_(initial_velocity * std::cos(alpha)),
        vy_(-initial_velocity * std::sin(alpha)) {}

  auto next_offset() -> Offset {
    auto dt = Tick::dt();

    auto dx = vx_ * dt;

    auto tick = tick_.next_tick();
    auto dy = vy_ * dt + 0.5F * g_ * dt * (tick * 2 - dt);

    return {.dx = dx, .dy = dy};
  };

 private:
  float vx_{};
  float vy_{};
  Tick tick_{};

  float g_ = DEFAULT_GRAVITY;
};
#endif
