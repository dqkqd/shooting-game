#ifndef PLAYER_H
#define PLAYER_H

#include <functional>

#include "SDL_render.h"
#include "components/physics.h"
#include "components/primitive.h"
#include "config.h"
#include "ecs/world.h"
#include "graphic.h"
#include "services/texture.h"

namespace player {

class NormalAnimation {
 public:
  auto next_sprite() -> int { return current_sprite_++; }
  auto position() -> TexturePosition {
    auto x = (next_sprite() / PLAYER_FRAMES_DELAY) % PLAYER_TOTAL_SPRITES;
    return TexturePosition{static_cast<float>(x * PLAYER_SPRITE_WIDTH), 0,
                           PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
  }

 private:
  int current_sprite_ = 0;
};

inline void init(Graphic& graphic, World& world) {
  auto start_position =
      RenderPosition{(GAME_WIDTH - PLAYER_SPRITE_WIDTH) / 2.0, 0,
                     PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
  world.spawn_entity_with(
      TextureManager::add_from_file_unchecked(graphic.renderer(), PLAYER_IMAGE),
      std::move(start_position), NormalAnimation(), Falling());

  std::function<void(
      Query<SDL_Texture*, RenderPosition, NormalAnimation, Falling>)>
      system = [&graphic](
                   Query<SDL_Texture*, RenderPosition, NormalAnimation, Falling>
                       query) {
        for (auto [texture, position, animation, falling] : query) {
          auto src = animation.position();
          position.rect.y += falling.vy();
          SDL_RenderTextureRotated(graphic.renderer(), texture, &src.rect,
                                   &position.rect, 0, NULL,
                                   SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
        }
      };
  world.add_system(std::move(system));
};

}  // namespace player

#endif
