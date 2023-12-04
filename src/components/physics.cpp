#include "components/physics.h"

#include <cmath>

Tick::Tick(float dt) : dt_{dt} {}

auto Tick::next_tick() -> float {
  tick_ += dt();
  return tick_;
}

auto Tick::dt() const -> float { return dt_; }

ProjectileMotion::ProjectileMotion(float initial_velocity, float alpha,
                                   float dt)
    : vx_(initial_velocity * std::cos(alpha)),
      vy_(initial_velocity * std::sin(alpha)),
      tick_{Tick{dt}} {}

auto ProjectileMotion::next_offset() -> Offset {
  auto dt = tick_.dt();

  auto dx = vx_ * dt;

  auto tick = tick_.next_tick();
  auto dy = vy_ * dt + 0.5F * g_ * dt * (tick * 2 - dt);

  return {.dx = dx, .dy = dy};
};

auto ProjectileMotion::movable() const -> bool { return vx_ == 0 && vy_ == 0; }
void ProjectileMotion::reset() {
  vx_ = 0;
  vy_ = 0;
}

void ProjectileMotion::change_x_direction() { vx_ = -vx_; }
void ProjectileMotion::change_y_direction() { vy_ = -vy_; }
