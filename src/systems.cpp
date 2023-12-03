#include "systems.h"

#include "components/position.h"
#include "player/player.h"
#include "player/shooter.h"

void shared_systems::render_system(World& world, Graphic& graphic) {
  auto query = world.query<SDL_Texture*, TexturePosition, RenderPosition>();
  for (auto [texture, src, dest] : query) {
    auto dest_rect = graphic.camera().get_position_for(dest).rect;
    SDL_RenderTexture(graphic.renderer(), texture, &src.rect, &dest_rect);
  }

  // render player shooting position
  for (auto [info, player_position, shoot_position] :
       world.query<PlayerInfo, RenderPosition, ShootPosition>()) {
    if (info.status == PlayerStatus::STOPPED) {
      auto player_dest_position =
          graphic.camera().get_position_for(player_position);
      auto top_right = player_dest_position.top_right();
      SDL_RenderLine(graphic.renderer(), top_right.x, top_right.y,
                     shoot_position.x, shoot_position.y);
    }
  }
};
