#include "player/player.h"

#include "components/animation.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "enemy/rhino.h"
#include "services/texture.h"

void Player::init(World& world, Graphic& graphic) {
  auto config = GameConfig::data().player;

  auto* texture = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                          config.image.c_str());
  auto texture_size = get_texture_size(texture);

  auto player_width = texture_size.w / static_cast<float>(config.total_sprites);
  auto player_height = texture_size.h;

  auto animation = TextureAnimation(config.frames_delay, player_width,
                                    player_height, config.total_sprites);
  auto texture_position = animation.next_position({});

  auto start_position = RenderPosition{static_cast<float>(config.position.x),
                                       static_cast<float>(config.position.y),
                                       player_width, player_height};

  world.spawn_entity_with(std::move(texture), std::move(texture_position),
                          std::move(start_position), std::move(animation),
                          ProjectileMotion(0, 0), PlayerInfo());
}

void Player::init_dead_player(World& world, Graphic& graphic) {
  auto* texture = TextureManager::add_from_file_unchecked(
      graphic.renderer(),
      "assets/pixel_adventure_assets/Enemies/Turtle/Hit (44x26).png");
  auto texture_size = get_texture_size(texture);

  auto player_width = texture_size.w / 5.0F;
  auto player_height = texture_size.h;

  auto animation = TextureAnimation(5, player_width, player_height, 5);
  auto start_position = RenderPosition{0, 0, player_width, player_height};

  auto texture_position = animation.next_position({.hidden = true});
  world.spawn_entity_with(std::move(texture), std::move(texture_position),
                          std::move(start_position), std::move(animation),
                          PlayerDeadInfo());
}

void Player::animation_system(World& world) {
  for (auto [query_texture, query_animation] :
       world.query<TexturePosition, TextureAnimation>()) {
    query_texture = query_animation.next_position(std::move(query_texture));
  }
}

auto advance_by_offset(World& world, RenderPosition& player_position,
                       Offset offset) -> Collision {
  auto collision = Collision{.x = false, .y = false};

  auto player_position_by_dx =
      player_position.with_x(player_position.rect.x + offset.dx);
  auto player_position_by_dy =
      player_position.with_y(player_position.rect.y + offset.dy);

  // checking collisions
  for (auto [position, _] : world.query<RenderPosition, Collidable>()) {
    if (!collision.x && player_position_by_dx.collide(position)) {
      collision.x = true;
    }
    if (!collision.y && player_position_by_dy.collide(position)) {
      collision.y = true;
    }

    if (collision.x && collision.y) {
      break;
    }
  }

  if (!collision.x) {
    player_position.rect.x += offset.dx;
  }

  if (!collision.y) {
    player_position.rect.y += offset.dy;
  }

  return collision;
}

void Player::moving_system(World& world) {
  for (auto [info, position, motion] :
       world.query<PlayerInfo, RenderPosition, ProjectileMotion>()) {
    if (info.status == PlayerStatus::STOPPED ||
        info.status == PlayerStatus::DEAD) {
      continue;
    }

    auto offset = motion.next_offset();

    auto collision = advance_by_offset(world, position, offset);

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
  for (auto [position, _] : world.query<RenderPosition, PlayerInfo>()) {
    camera.center_to(position);
  }
}

auto Player::should_dead(World& world, RenderPosition& position) -> bool {
  if (position.rect.y > static_cast<float>(GameConfig::data().level.height)) {
    return true;
  }

  for (auto [rhino_position, rhino_info] :
       world.query<RenderPosition, RhinoInfo>()) {
    if (position.collide(rhino_position)) {
      return true;
    }
  }
  return false;
}

void Player::make_player_dead(World& world, RenderPosition& position,
                              PlayerInfo& info) {
  info.status = PlayerStatus::DEAD;

  for (auto [player_dead_src_position, player_dead_dest_position,
             player_dead_info] :
       world.query<TexturePosition, RenderPosition, PlayerDeadInfo>()) {
    player_dead_src_position.hidden = false;
    player_dead_dest_position.rect = position.rect;
  }
}

void Player::restart_player(World& world) {
  for (auto [src, position, motion, info] :
       world.query<TexturePosition, RenderPosition, ProjectileMotion,
                   PlayerInfo>()) {
    info.status = PlayerStatus::IDLE;
    src.hidden = false;
    position.rect.x = static_cast<float>(GameConfig::data().player.position.x);
    position.rect.y = static_cast<float>(GameConfig::data().player.position.y);
    motion.reset();
  }

  for (auto [player_dead_src_position, player_dead_dest_position,
             player_dead_info] :
       world.query<TexturePosition, RenderPosition, PlayerDeadInfo>()) {
    player_dead_src_position.hidden = true;
    player_dead_dest_position.rect = {};
  }
}
