#include "systems.h"

#include "SDL_log.h"
#include "components/position.h"
#include "game_state.h"
#include "player/player.h"
#include "player/shooter.h"

auto should_show_shooter_indicator(const ShooterInfo& shooter,
                                   const PlayerInfo& player) -> bool {
  return player.status == PlayerStatus::STOPPED && !shooter.hidden;
}

void shared_systems::render_system(World& world, Graphic& graphic) {
  for (auto [texture, src, dest] :
       world.query<SDL_Texture*, TexturePosition, RenderPosition>()) {
    if (src.hidden) {
      continue;
    }

    auto dest_rect = graphic.camera().get_position_for(dest).rect;
    SDL_RenderTextureRotated(graphic.renderer(), texture, &src.rect, &dest_rect,
                             0, NULL, src.flip);
  }

  // render player shooting position
  for (auto [info, player_position] :
       world.query<PlayerInfo, RenderPosition>()) {
    for (auto [shooter_info] : world.query<ShooterInfo>()) {
      if (should_show_shooter_indicator(shooter_info, info)) {
        for (auto point : shooter_info.points) {
          RenderPosition dest{point.x, point.y,
                              shooter_info.src_position.rect.w,
                              shooter_info.src_position.rect.h};
          SDL_RenderTexture(graphic.renderer(), shooter_info.indicator_texture,
                            &shooter_info.src_position.rect, &dest.rect);
        }
      }
    }
  }

  // render game over
  for (auto [info] : world.query<GameInfo>()) {
    if (info.status == GameStatus::GAME_OVER) {
      SDL_Log("GAME OVER");
    }
  }
};
