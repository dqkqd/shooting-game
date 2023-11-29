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

  std::function<void(World&)> falling_system = [](World& world) {
    auto player_query =
        world.query().get_or_add<TexturePosition, RenderPosition, Falling>(
            world.archetypes());
    auto tile_query = world.query().get_or_add<RenderPosition, Collidable>(
        world.archetypes());

    for (auto [player_texture_position, player_position, falling] :
         player_query) {
      auto vy = falling.vy();

      // find the best vy using binary search
      float lv = 0;
      float rv = vy;
      while (lv + PIXEL_FLOAT_OFFSET < rv) {
        float mv = (lv + rv) / 2;
        auto pos = player_position;
        pos.rect.y += mv;

        bool collided =
            std::any_of(tile_query.begin(), tile_query.end(), [&pos](auto it) {
              return pos.collide(std::get<RenderPosition&>(it));
            });

        if (collided) {
          rv = mv;
        } else {
          lv = mv;
        }
      }

      player_position.rect.y += lv;
    }
  };

  world.add_system(std::move(animation_system));
  world.add_system(std::move(falling_system));
};
