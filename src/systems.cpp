#include "systems.h"

#include "components/position.h"
#include "player/player.h"
#include "player/shooter.h"

void shared_systems::render_system(World& world, Graphic& graphic) {
  auto query = world.query<SDL_Texture*, TexturePosition, RenderPosition>();
  for (auto [texture, src, dest] : query) {
    auto dest_rect = graphic.camera().get_position_for(dest).rect;
    SDL_RenderTextureRotated(graphic.renderer(), texture, &src.rect, &dest_rect,
                             0, NULL, src.flip);
  }

  // render player shooting position
  for (auto [info, player_position, shooter_info] :
       world.query<PlayerInfo, RenderPosition, ShooterInfo>()) {
    if (info.status == PlayerStatus::STOPPED) {
      auto player_dest_position =
          graphic.camera().get_position_for(player_position);
      auto top_right = player_dest_position.top_right();
      SDL_RenderLine(graphic.renderer(), top_right.x, top_right.y,
                     shooter_info.point.x, shooter_info.point.y);
    }
  }
};
