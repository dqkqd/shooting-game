#include "player/player.h"

#include "components/animation.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "enemy/rhino.h"
#include "services/texture.h"
#include "utils.h"

void Player::init(World& world, Graphic& graphic) {
  auto config = GameConfig::data().player;
  auto [texture, texture_info, render_info, animation] =
      game_common::load_sprite(config, graphic);

  render_info.rect.x = static_cast<float>(config.position.x);
  render_info.rect.y = static_cast<float>(config.position.y);
  world.spawn_entity_with(std::move(texture), std::move(texture_info),
                          std::move(render_info), std::move(animation),
                          ProjectileMotion(0, 0), PlayerInfo());
}

void Player::init_dead_player(World& world, Graphic& graphic) {
  auto config = GameConfig::data().player.dead_state;

  auto* texture = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                          config.image.c_str());
  auto texture_size = get_texture_size(texture);

  auto player_width = texture_size.w / static_cast<float>(config.total_sprites);
  auto player_height = texture_size.h;

  auto animation = TextureAnimation(config.frames_delay, player_width,
                                    player_height, config.total_sprites);
  auto texture_info = animation.next_render_info({.hidden = true});

  auto render_info = RenderInfo{0, 0, player_width, player_height};

  world.spawn_entity_with(std::move(texture), std::move(texture_info),
                          std::move(render_info), std::move(animation),
                          PlayerDeadInfo());
}

void Player::animation_system(World& world) {
  for (auto [texture_info, query_animation] :
       world.query<TextureInfo, TextureAnimation>()) {
    texture_info = query_animation.next_render_info(std::move(texture_info));
  }
}

auto advance_by_offset(World& world, RenderInfo& render_info, Offset offset)
    -> Collision {
  auto collision = Collision{.x = false, .y = false};

  auto player_position_by_dx =
      render_info.with_x(render_info.rect.x + offset.dx);
  auto player_position_by_dy =
      render_info.with_y(render_info.rect.y + offset.dy);

  // checking collisions
  for (auto [tile_render_info, _] : world.query<RenderInfo, Collidable>()) {
    if (!collision.x && player_position_by_dx.collide(tile_render_info)) {
      collision.x = true;
    }
    if (!collision.y && player_position_by_dy.collide(tile_render_info)) {
      collision.y = true;
    }

    if (collision.x && collision.y) {
      break;
    }
  }

  if (!collision.x) {
    render_info.rect.x += offset.dx;
  }

  if (!collision.y) {
    render_info.rect.y += offset.dy;
  }

  return collision;
}

void Player::moving_system(World& world) {
  for (auto [info, render_info, motion] :
       world.query<PlayerInfo, RenderInfo, ProjectileMotion>()) {
    if (info.status == PlayerStatus::STOPPED ||
        info.status == PlayerStatus::DEAD) {
      continue;
    }

    auto offset = motion.next_offset();

    auto collision = advance_by_offset(world, render_info, offset);

    if (collision.x) {
      motion.change_x_direction();
    }

    if (collision.y) {
      motion.change_y_direction();

      // We should stop if we are falling down and collided
      if (offset.dy > 0) {
        info.status = PlayerStatus::STOPPED;
        motion.reset();
      }
    }
  }
};

void Player::camera_system(World& world, Camera& camera) {
  for (auto [render_info, _] : world.query<RenderInfo, PlayerInfo>()) {
    camera.center_to(render_info);
  }
}

auto Player::should_dead(World& world, RenderInfo& render_info) -> bool {
  if (render_info.rect.y >
      static_cast<float>(GameConfig::data().level.height)) {
    return true;
  }

  for (auto [rhino_render_info, rhino_info] :
       world.query<RenderInfo, RhinoInfo>()) {
    if (render_info.collide(rhino_render_info)) {
      return true;
    }
  }
  return false;
}

auto Player::try_make_player_dead(World& world) -> bool {
  for (auto [texture_info, render_info, info] :
       world.query<TextureInfo, RenderInfo, PlayerInfo>()) {
    if (should_dead(world, render_info)) {
      info.status = PlayerStatus::DEAD;
      texture_info.hidden = true;

      for (auto [dead_texture_info, dead_render_info, dead_info] :
           world.query<TextureInfo, RenderInfo, PlayerDeadInfo>()) {
        dead_texture_info.hidden = false;
        dead_render_info.rect = render_info.rect;
      }
      return true;
    }
  }

  return false;
}

void Player::restart_player(World& world) {
  for (auto [texture_info, render_info, motion, info] :
       world.query<TextureInfo, RenderInfo, ProjectileMotion, PlayerInfo>()) {
    info.status = PlayerStatus::IDLE;
    texture_info.hidden = false;
    render_info.rect.x =
        static_cast<float>(GameConfig::data().player.position.x);
    render_info.rect.y =
        static_cast<float>(GameConfig::data().player.position.y);
    motion.reset();
  }

  for (auto [texture_info, render_info, info] :
       world.query<TextureInfo, RenderInfo, PlayerDeadInfo>()) {
    texture_info.hidden = true;
    render_info.rect = {};
  }
}
