#include "player/shooter.h"

#include <cmath>

#include "camera.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "player/player.h"

auto calculate_initial_velocity(SDL_FPoint start, SDL_FPoint end) -> float {
  auto dx = end.x - start.x;
  auto dy = end.y - start.y;

  auto dx2 = dx * dx;
  auto dy2 = dy * dy;

  auto distance = std::sqrt(dx2 + dy2);

  return std::clamp(distance / 2.5F, 0.0F, 120.0F);
}

void Shooter::init(World& world, Graphic& graphic) {
  world.spawn_entity_with(ShooterInfo());
}

void Shooter::shoot_system(World& world, SDL_Event event, Camera& camera) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    for (auto [info, position, motion] :
         world.query<PlayerInfo, RenderPosition, ProjectileMotion>()) {
      if (info.status == PlayerStatus::SELF_SHOOTING) {
        continue;
      }

      info.status = PlayerStatus::SELF_SHOOTING;

      auto player_dest_position = camera.get_position_for(position);
      auto px =
          player_dest_position.rect.x + player_dest_position.rect.w / 2.0F;
      auto py =
          player_dest_position.rect.y + player_dest_position.rect.h / 2.0F;

      auto alpha = std::atan2(event.button.y - py, event.button.x - px);
      auto velocity = calculate_initial_velocity(
          {event.button.x, event.button.y}, {px, py});
      motion = ProjectileMotion(velocity, alpha);
    }
  }
}

void Shooter::assign_position_system(World& world, SDL_Event event,
                                     Camera& camera) {
  for (auto [position, player_info] :
       world.query<RenderPosition, PlayerInfo>()) {
    for (auto [shooter_info] : world.query<ShooterInfo>()) {
      if (player_info.status != PlayerStatus::STOPPED) {
        shooter_info.hidden = true;
        continue;
      }

      shooter_info.hidden = false;

      auto player_dest_position = camera.get_position_for(position);
      auto px =
          player_dest_position.rect.x + player_dest_position.rect.w / 2.0F;
      auto py =
          player_dest_position.rect.y + player_dest_position.rect.h / 2.0F;

      auto alpha = std::atan2(event.button.y - py, event.button.x - px);

      auto velocity = calculate_initial_velocity(
          {event.button.x, event.button.y}, {px, py});
      auto motion = ProjectileMotion(velocity, alpha,
                                     GameConfig::data().player.shooter.dt);

      shooter_info.points[0] = {px, py};

      for (std::size_t i = 1; i < shooter_info.points.size(); ++i) {
        auto offset = motion.next_offset();
        shooter_info.points[i] = {shooter_info.points[i - 1].x + offset.dx,
                                  shooter_info.points[i - 1].y + offset.dy};
      }
    }
  }
}
