#include "player/shooter.h"

#include "camera.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "graphic.h"
#include "player/player.h"
#include "services/texture.h"

auto get_projectile_motion(const RenderPosition& position,
                           SDL_FPoint mouse_position, float dt)
    -> ProjectileMotion {
  auto info = GameConfig::data().player.shooter;
  auto motion_init_value =
      MotionInitializer::calculate(position.center(), mouse_position,
                                   info.velocity_scale, info.max_velocity);
  return ProjectileMotion{motion_init_value.velocity, motion_init_value.alpha,
                          dt};
}

void Shooter::init(World& world, Graphic& graphic) {
  auto* texture = TextureManager::add_from_file_unchecked(
      graphic.renderer(),
      GameConfig::data().player.shooter.indicator_image.c_str());
  auto texture_size = get_texture_size(texture);
  auto texture_position =
      TexturePosition{{0, 0, texture_size.w, texture_size.h}};
  world.spawn_entity_with(ShooterInfo{.indicator_texture = texture,
                                      .src_position = texture_position});
}

void Shooter::shoot_system(World& world, SDL_Event event, Camera& camera) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    for (auto [info, position, motion] :
         world.query<PlayerInfo, RenderPosition, ProjectileMotion>()) {
      if (info.status == PlayerStatus::SELF_SHOOTING) {
        continue;
      }

      info.status = PlayerStatus::SELF_SHOOTING;

      motion = get_projectile_motion(camera.get_position_for(position),
                                     {event.button.x, event.button.y},
                                     GameConfig::data().physics.dt);
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

      auto player_position = camera.get_position_for(position);
      auto motion = get_projectile_motion(player_position,
                                          {event.button.x, event.button.y},
                                          GameConfig::data().player.shooter.dt);

      shooter_info.points[0] = player_position.center();
      for (std::size_t i = 1; i < shooter_info.points.size(); ++i) {
        auto offset = motion.next_offset();
        shooter_info.points[i] = {shooter_info.points[i - 1].x + offset.dx,
                                  shooter_info.points[i - 1].y + offset.dy};
      }
    }
  }
}

auto should_show_shooter_indicator(const ShooterInfo& shooter,
                                   const PlayerInfo& player) -> bool {
  return player.status == PlayerStatus::STOPPED && !shooter.hidden;
}

void Shooter::indicator_render_system(World& world, Graphic& graphic) {
  for (auto [info, player_position] :
       world.query<PlayerInfo, RenderPosition>()) {
    for (auto [shooter_info] : world.query<ShooterInfo>()) {
      if (should_show_shooter_indicator(shooter_info, info)) {
        for (auto point : shooter_info.points) {
          RenderPosition dest{point.x, point.y,
                              shooter_info.src_position.rect.w,
                              shooter_info.src_position.rect.h};
          SDL_RenderTexture(graphic.renderer(), shooter_info.indicator_texture,
                            &shooter_info.src_position.rect, &dest.rect);
        }
      }
    }
  }
}
