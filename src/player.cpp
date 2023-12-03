#include "player.h"

#include "SDL_events.h"
#include "components/animation.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "services/texture.h"

void player::init(Graphic& graphic, World& world) {
  auto* texture = TextureManager::add_from_file_unchecked(
      graphic.renderer(), GameConfig::data().player.image.c_str());
  auto texture_size = get_texture_size(texture);

  auto player_width =
      texture_size.w /
      static_cast<float>(GameConfig::data().player.total_sprites);
  auto player_height = texture_size.h;

  auto animation =
      TextureAnimation(GameConfig::data().player.frames_delay, player_width,
                       player_height, GameConfig::data().player.total_sprites);
  auto texture_position = animation.next_position();

  auto start_position = RenderPosition{
      (static_cast<float>(GameConfig::data().graphic.width) - player_width) /
          5.0F,
      static_cast<float>(GameConfig::data().graphic.height) * 2.0F / 3.0F,
      player_width, player_height};

  world.spawn_entity_with(std::move(texture), std::move(texture_position),
                          std::move(start_position), std::move(animation),
                          ProjectileMotion(0, 0), ShootPosition(), Player());
}

void player::animation_system(World& world) {
  auto query = world.query<TexturePosition, TextureAnimation>();
  for (auto [query_texture, query_animation] : query) {
    query_texture = query_animation.next_position();
  }
}

auto player::advance_by_offset(World& world, RenderPosition& player_position,
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

void player::moving_system(World& world) {
  for (auto [character, player_position, projectile_motion] :
       world.query<Player, RenderPosition, ProjectileMotion>()) {
    if (character.status == Status::STOPPED) {
      continue;
    }

    auto offset = projectile_motion.next_offset();

    auto collision = advance_by_offset(world, player_position, offset);

    if (collision.x) {
      projectile_motion.change_x_direction();
    }

    if (collision.y) {
      projectile_motion.change_y_direction();

      // We should stop if we are falling down and collided
      if (offset.dy > 0) {
        character.status = Status::STOPPED;
        projectile_motion.reset();
      }
    }
  }
};

void player::camera_system(World& world, Camera& camera) {
  auto query = world.query<RenderPosition, Player>();
  for (auto [position, _] : query) {
    camera.center_to(position);
  }
}

void player::shoot_system(World& world, SDL_Event event, Camera& camera) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    for (auto [character, player_position, projectile_motion] :
         world.query<Player, RenderPosition, ProjectileMotion>()) {
      if (character.status == Status::SELF_SHOOTING) {
        continue;
      }

      character.status = Status::SELF_SHOOTING;

      auto player_dest_position = camera.get_position_for(player_position);
      auto px =
          player_dest_position.rect.x + player_dest_position.rect.w / 2.0F;
      auto py =
          player_dest_position.rect.y + player_dest_position.rect.h / 2.0F;

      auto alpha = std::atan2(event.button.y - py, event.button.x - px);
      auto velocity = 100.0F;
      projectile_motion = ProjectileMotion(velocity, alpha);
    }
  }
}

void player::assign_shoot_position(World& world, SDL_Event event) {
  for (auto [player, position] : world.query<Player, ShootPosition>()) {
    if (player.status == Status::STOPPED) {
      position = ShootPosition{.x = event.button.x, .y = event.button.y};
    }
  }
}
