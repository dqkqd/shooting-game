#include "player.h"

#include "SDL_events.h"
#include "components/animation.h"
#include "components/physics.h"
#include "components/position.h"
#include "services/texture.h"

void player::init(Graphic& graphic, World& world) {
  auto* texture =
      TextureManager::add_from_file_unchecked(graphic.renderer(), PLAYER_IMAGE);
  auto texture_size = get_texture_size(texture);

  auto player_width = texture_size.w / PLAYER_TOTAL_SPRITES;
  auto player_height = texture_size.h;

  auto animation = TextureAnimation(PLAYER_FRAMES_DELAY, player_width,
                                    player_height, PLAYER_TOTAL_SPRITES);
  auto texture_position = animation.next_position();

  auto start_position =
      RenderPosition{(GAME_WIDTH - player_width) / 5.0F,
                     GAME_HEIGHT * 2.0F / 3.0F, player_width, player_height};

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

void player::moving_system(World& world) {
  for (auto [character, player_position, projectile_motion] :
       world.query<Player, RenderPosition, ProjectileMotion>()) {
    if (character.status == Status::STOPPED) {
      continue;
    }

    auto offset = projectile_motion.next_offset();
    player_position.rect.x += offset.dx;
    player_position.rect.y += offset.dy;

    auto collided = false;
    for (auto [tile_position, _] : world.query<RenderPosition, Collidable>()) {
      if (player_position.collide(tile_position)) {
        collided = true;
        break;
      }
    }

    if (!collided) {
      continue;
    }

    // collided, need to revert changes, find the best offset and stop moving
    character.status = Status::STOPPED;
    projectile_motion.reset();

    player_position.rect.x -= offset.dx;
    player_position.rect.y -= offset.dy;
    for (auto [tile_position, _] : world.query<RenderPosition, Collidable>()) {
      offset = player_position.closest_offset(tile_position, offset);
    }
    player_position.rect.x += offset.dx;
    player_position.rect.y += offset.dy;
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
