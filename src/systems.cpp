#include "systems.h"

#include "components/position.h"
#include "player/player.h"
#include "player/shooter.h"

auto should_show_shooter_indicator(const ShooterInfo& shooter,
                                   const PlayerInfo& player) -> bool {
  return player.status == PlayerStatus::STOPPED && !shooter.hidden;
}

void shared_systems::render_system(World& world, Graphic& graphic) {
  auto query = world.query<SDL_Texture*, TexturePosition, RenderPosition>();
  for (auto [texture, src, dest] : query) {
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
        SDL_RenderPoints(graphic.renderer(), shooter_info.points.data(),
                         static_cast<int>(shooter_info.points.size()));
      }
    }
  }
};
