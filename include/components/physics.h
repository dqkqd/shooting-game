#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include "SDL_rect.h"
#include "components/position.h"
#include "config.h"

struct Collidable {};

class Tick {
 public:
  explicit Tick(float dt);

  auto next_tick() -> float;
  [[nodiscard]] auto dt() const -> float;

 private:
  float dt_;
  float tick_{};
};

class MotionInitializer {
 public:
  struct VelocityAndAlpha {
    float velocity;
    float alpha;
  };

  static auto calculate(SDL_FPoint start, SDL_FPoint end, float velocity_scale,
                        float max_velocity) -> VelocityAndAlpha;
};

class ProjectileMotion {
 public:
  ProjectileMotion(float initial_velocity, float alpha,
                   float dt = GameConfig::data().physics.dt);

  auto next_offset() -> Offset;
  [[nodiscard]] auto movable() const -> bool;
  void reset();
  void shutdown();

  void change_x_direction();
  void change_y_direction();

 private:
  float vx_{};
  float vy_{};
  Tick tick_;

  float g_ = GameConfig::data().physics.gravity;
};
#endif
