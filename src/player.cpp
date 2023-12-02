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
                          ProjectileMotion(0, 0), ShootPosition(), IsPlayer());
}

void player::animation_system(World& world) {
  auto query = world.query<TexturePosition, TextureAnimation>();
  for (auto [query_texture, query_animation] : query) {
    query_texture = query_animation.next_position();
  }
}

void player::moving_system(World& world) {
  auto player_query = world.query<RenderPosition, ProjectileMotion>();
  auto tile_query = world.query<RenderPosition, Collidable>();

  for (auto [player_position, projectile_motion] : player_query) {
    auto offset = projectile_motion.next_offset();

    for (auto [tile_position, _] : tile_query) {
      offset = player_position.closest_offset(tile_position, offset);
    }

    player_position.rect.x += offset.dx;
    player_position.rect.y += offset.dy;
  }
};

void player::camera_system(World& world, Camera& camera) {
  auto query = world.query<RenderPosition, IsPlayer>();
  for (auto [position, _] : query) {
    camera.center_to(position);
  }
}

void player::shoot_system(World& world, SDL_Event event, Camera& camera) {
  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    for (auto [player_position, projectile_motion, _] :
         world.query<RenderPosition, ProjectileMotion, IsPlayer>()) {
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
  for (auto [position, _] : world.query<ShootPosition, IsPlayer>()) {
    position =
        ShootPosition{.x = event.button.x, .y = event.button.y, .show = true};
  }
}
