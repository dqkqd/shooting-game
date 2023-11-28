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
  auto start_position =
      RenderPosition{(GAME_WIDTH - PLAYER_SPRITE_WIDTH) / 2.0, 0,
                     PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
  world.spawn_entity_with(
      TextureManager::add_from_file_unchecked(graphic.renderer(), PLAYER_IMAGE),
      std::move(start_position), NormalAnimation(), Falling());

  std::function<void()> system = [&world, &graphic]() {
    auto player_query =
        world.query()
            .get_or_add<SDL_Texture*, RenderPosition, NormalAnimation, Falling>(
                world.archetypes());
    auto tile_query =
        world.query().get_or_add<SDL_Texture*, RenderPosition, CollidableTile>(
            world.archetypes());

    for (auto [player_texture, player_position, player_animation, falling] :
         player_query) {
      auto src = player_animation.position();
      auto vy = falling.vy();
      player_position.rect.y += vy;

      for (auto [tile_texture, tile_position, _] : tile_query) {
        if (player_position.collide(tile_position)) {
          player_position.rect.y -= vy;
          break;
        }
      }

      SDL_RenderTextureRotated(graphic.renderer(), player_texture, &src.rect,
                               &player_position.rect, 0, NULL,
                               SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
    }
  };

  world.add_system(std::move(system));
};
