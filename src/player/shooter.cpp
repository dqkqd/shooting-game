#include "player/shooter.h"

#include "components/physics.h"
#include "player/player.h"

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
      auto velocity = 100.0F;
      motion = ProjectileMotion(velocity, alpha);
    }
  }
}

void Shooter::assign_position_system(World& world, SDL_Event event) {
  for (auto [info, shooter_info] : world.query<PlayerInfo, ShooterInfo>()) {
    if (info.status == PlayerStatus::STOPPED) {
      shooter_info = ShooterInfo{.point = {event.button.x, event.button.y}};
    }
  }
}
