#include "player.h"

#include "SDL_rect.h"
#include "components/physics.h"
#include "components/primitive.h"
#include "config.h"
#include "services/texture.h"
#include "tiles/tilemap.h"

auto player::NormalAnimation::next_sprite() -> int { return current_sprite_++; }
auto player::NormalAnimation::position() -> TexturePosition {
  auto x = (next_sprite() / PLAYER_FRAMES_DELAY) % PLAYER_TOTAL_SPRITES;
  return TexturePosition{static_cast<float>(x * PLAYER_SPRITE_WIDTH), 0,
                         PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
}

void player::init(Graphic& graphic, World& world) {
  auto animation = NormalAnimation();
  auto texture_position = animation.position();
  auto start_position =
      RenderPosition{(GAME_WIDTH - PLAYER_SPRITE_WIDTH) / 2.0, 0,
                     PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
  world.spawn_entity_with(
      TextureManager::add_from_file_unchecked(graphic.renderer(), PLAYER_IMAGE),
      std::move(texture_position), std::move(start_position),
      std::move(animation), Falling());

  std::function<void(Query<TexturePosition, NormalAnimation>)>
      animation_system = [](Query<TexturePosition, NormalAnimation> query) {
        for (auto [texture, query_animation] : query) {
          texture = query_animation.position();
        }
      };

  std::function<void()> falling_system = [&world]() {
    auto player_query =
        world.query().get_or_add<TexturePosition, RenderPosition, Falling>(
            world.archetypes());
    auto tile_query = world.query().get_or_add<RenderPosition, CollidableTile>(
        world.archetypes());

    for (auto [player_texture_position, player_position, falling] :
         player_query) {
      auto vy = falling.vy();
      player_position.rect.y += vy;

      for (auto [tile_position, _] : tile_query) {
        if (player_position.collide(tile_position)) {
          player_position.rect.y -= vy;
          break;
        }
      }
    }
  };

  world.add_system(std::move(animation_system));
  world.add_system(std::move(falling_system));
};
