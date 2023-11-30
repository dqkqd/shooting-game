#include "player.h"

#include "SDL_rect.h"
#include "components/physics.h"
#include "components/primitive.h"
#include "config.h"
#include "services/texture.h"

auto player::NormalAnimation::next_sprite() -> int { return current_sprite_++; }
auto player::NormalAnimation::position() -> TexturePosition {
  auto x = (next_sprite() / PLAYER_FRAMES_DELAY) % PLAYER_TOTAL_SPRITES;
  return TexturePosition{static_cast<float>(x * PLAYER_SPRITE_WIDTH), 0,
                         PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
}

void player::init(Graphic& graphic, World& world) {
  auto animation = NormalAnimation();
  auto texture_position = animation.position();
  auto start_position = RenderPosition{
      (GAME_WIDTH - PLAYER_SPRITE_WIDTH) / 5.0, GAME_HEIGHT * 2.0 / 3.0,
      PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
  world.spawn_entity_with(
      TextureManager::add_from_file_unchecked(graphic.renderer(), PLAYER_IMAGE),
      std::move(texture_position), std::move(start_position),
      std::move(animation), ProjectileMotion(30, 3.14 * 6 / 13));

  std::function<void(Query<TexturePosition, NormalAnimation>)>
      animation_system = [](Query<TexturePosition, NormalAnimation> query) {
        for (auto [texture, query_animation] : query) {
          texture = query_animation.position();
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
        offset.dy = player_position.best_y_offset(tile_position, offset.dy);
        offset.dx = player_position.best_x_offset(tile_position, offset.dx);
      }

      player_position.rect.x += offset.dx;
      player_position.rect.y += offset.dy;
    }
  };

  world.add_system(std::move(animation_system));
  world.add_system(std::move(falling_system));
};
