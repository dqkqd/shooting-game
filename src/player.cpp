#include "player.h"

#include "SDL_rect.h"
#include "components/animation.h"
#include "components/physics.h"
#include "components/position.h"
#include "config.h"
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
                          ProjectileMotion(100, 3.14 * 6 / 13), IsPlayer());

  std::function<void(Query<TexturePosition, TextureAnimation>)>
      animation_system = [](Query<TexturePosition, TextureAnimation> query) {
        for (auto [query_texture, query_animation] : query) {
          query_texture = query_animation.next_position();
        }
      };

  std::function<void(World&)> falling_system = [](World& world) {
    auto player_query =
        world.query()
            .get_or_add<TexturePosition, RenderPosition, ProjectileMotion>(
                world.archetypes());

    auto tile_query = world.query().get_or_add<RenderPosition, Collidable>(
        world.archetypes());

    for (auto [player_texture_position, player_position, projectile_motion] :
         player_query) {
      auto offset = projectile_motion.next_offset();

      for (auto [tile_position, _] : tile_query) {
        offset = player_position.closest_offset(tile_position, offset);
      }

      player_position.rect.x += offset.dx;
      player_position.rect.y += offset.dy;
    }
  };

  std::function<void(Query<RenderPosition, IsPlayer>)> set_camera =
      [&graphic](Query<RenderPosition, IsPlayer> query) {
        for (auto [position, _] : query) {
          graphic.camera().center_to(position);
        }
      };

  world.add_system(std::move(animation_system));
  world.add_system(std::move(falling_system));
  world.add_system(std::move(set_camera));
}
