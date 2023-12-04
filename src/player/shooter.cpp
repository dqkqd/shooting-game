#include "player/shooter.h"

#include "camera.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "player/player.h"

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
      auto velocity = 100.0F;
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

      // TODO(khanhdq): scale velocity
      auto velocity = 100.0F;
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
