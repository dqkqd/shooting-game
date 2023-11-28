#ifndef PLAYER_H
#define PLAYER_H

#include <functional>

#include "SDL_render.h"
#include "components/primitive.h"
#include "components/texture.h"
#include "config.h"
#include "ecs/world.h"
#include "graphic.h"

using PlayerPosition = Position;

namespace player {

class NormalAnimation {
 public:
  auto next_sprite() -> int { return current_sprite_++; }
  auto position() -> Position {
    auto x = (next_sprite() / PLAYER_FRAMES_DELAY) % PLAYER_TOTAL_SPRITES;
    return Position{static_cast<float>(x * PLAYER_SPRITE_WIDTH), 0,
                    PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
  }

 private:
  int current_sprite_ = 0;
};

inline void init(Graphic& graphic, World& world) {
  world.spawn_entity_with<SDL_Texture*, NormalAnimation>(
      TextureManager::add_from_file_unchecked(graphic.renderer(), PLAYER_IMAGE),
      NormalAnimation());

  std::function<void(Query<SDL_Texture*, NormalAnimation>)> system =
      [&graphic](Query<SDL_Texture*, NormalAnimation> query) {
        for (auto [texture, animation] : query) {
          auto src = animation.position();
          auto dest = Position{(GAME_WIDTH - PLAYER_SPRITE_WIDTH) / 2.0, 466,
                               PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT};
          SDL_RenderTextureRotated(graphic.renderer(), texture, &src.rect,
                                   &dest.rect, 0, NULL,
                                   SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
        }
      };
  world.add_system(std::move(system));
};

}  // namespace player

#endif
