#include "player/shooter.h"

#include "SDL_mouse.h"
#include "camera.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "graphic.h"
#include "player/bullet.h"
#include "player/player.h"
#include "services/texture.h"

auto get_projectile_motion(const RenderInfo& render_info,
                           SDL_FPoint mouse_position, float dt)
    -> ProjectileMotion {
  auto info = GameConfig::data().player.shooter;
  auto motion_init_value =
      MotionInitializer::calculate(render_info.center(), mouse_position,
                                   info.velocity_scale, info.max_velocity);
  return ProjectileMotion{motion_init_value.velocity, motion_init_value.alpha,
                          dt};
}

void Shooter::init(World& world, Graphic& graphic) {
  auto* texture = TextureManager::add_from_file_unchecked(
      graphic.renderer(),
      GameConfig::data().player.shooter.indicator_image.c_str());
  auto texture_size = get_texture_size(texture);
  auto texture_info = TextureInfo{{0, 0, texture_size.w, texture_size.h}};
  world.spawn_entity_with(
      ShooterInfo{.indicator_texture = texture, .texture_info = texture_info});
}

void Shooter::shoot_system(World& world, SDL_Event event, Camera& camera) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    for (auto [player_info, player_render_info, player_motion] :
         world.query<PlayerInfo, RenderInfo, ProjectileMotion>()) {
      if (player_info.status != PlayerStatus::STOPPED) {
        continue;
      }

      auto motion = get_projectile_motion(
          camera.get_render_info_for(player_render_info),
          {event.button.x, event.button.y}, GameConfig::data().physics.dt);

      if (event.button.button == SDL_BUTTON_RIGHT) {
        player_info.status = PlayerStatus::SELF_SHOOTING;
        player_motion = motion;
      } else if (event.button.button == SDL_BUTTON_LEFT) {
        for (auto [bullet_info, bullet_texture_info, bullet_render_info,
                   bullet_motion] :
             world.query<BulletInfo, TextureInfo, RenderInfo,
                         ProjectileMotion>()) {
          if (bullet_info.status != BulletStatus::SHOOT) {
            bullet_info.status = BulletStatus::SHOOT;
            bullet_texture_info.hidden = false;
            bullet_render_info.rect.x = player_render_info.rect.x;
            bullet_render_info.rect.y = player_render_info.rect.y;
            bullet_motion = motion;
          }
        }
      }
    }
  }
}

void Shooter::assign_position_system(World& world, SDL_Event event,
                                     Camera& camera) {
  for (auto [player_render_info, player_info] :
       world.query<RenderInfo, PlayerInfo>()) {
    for (auto [bullet_render_info, bullet_info] :
         world.query<RenderInfo, BulletInfo>()) {
      for (auto [shooter_info] : world.query<ShooterInfo>()) {
        if (player_info.status == PlayerStatus::STOPPED &&
            bullet_info.status == BulletStatus::HIDDEN) {
          shooter_info.hidden = false;

          auto player_render_info_by_camera =
              camera.get_render_info_for(player_render_info);
          auto motion = get_projectile_motion(
              player_render_info_by_camera, {event.button.x, event.button.y},
              GameConfig::data().player.shooter.dt);

          shooter_info.points[0] = player_render_info_by_camera.center();
          for (std::size_t i = 1; i < shooter_info.points.size(); ++i) {
            auto offset = motion.next_offset();
            shooter_info.points[i] = {shooter_info.points[i - 1].x + offset.dx,
                                      shooter_info.points[i - 1].y + offset.dy};
          }
        } else {
          shooter_info.hidden = true;
        }
      }
    }
  }
}

auto should_show_shooter_indicator(const ShooterInfo& shooter,
                                   const PlayerInfo& player_info) -> bool {
  return player_info.status == PlayerStatus::STOPPED && !shooter.hidden;
}

void Shooter::indicator_render_system(World& world, Graphic& graphic) {
  for (auto [player_info, player_render_info] :
       world.query<PlayerInfo, RenderInfo>()) {
    for (auto [shooter_info] : world.query<ShooterInfo>()) {
      if (should_show_shooter_indicator(shooter_info, player_info)) {
        for (auto point : shooter_info.points) {
          RenderInfo render_info{point.x, point.y,
                                 shooter_info.texture_info.rect.w,
                                 shooter_info.texture_info.rect.h};
          SDL_RenderTexture(graphic.renderer(), shooter_info.indicator_texture,
                            &shooter_info.texture_info.rect, &render_info.rect);
        }
      }
    }
  }
}
