#include "components/physics.h"

#include <cmath>

#include "config.h"

auto Tick::next_tick() -> float {
  tick_ += dt();
  return tick_;
}
auto Tick::dt() -> float { return TICK_OFFSET; }

ProjectileMotion::ProjectileMotion(float initial_velocity,
                                   float alpha)  // NOLINT
    : vx_(initial_velocity * std::cos(alpha)),
      vy_(-initial_velocity * std::sin(alpha)) {}

auto ProjectileMotion::next_offset() -> Offset {
  auto dt = Tick::dt();

  auto dx = vx_ * dt;

  auto tick = tick_.next_tick();
  auto dy = vy_ * dt + 0.5F * g_ * dt * (tick * 2 - dt);

  return {.dx = dx, .dy = dy};
};
