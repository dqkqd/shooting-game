#include "systems.h"

#include "components/position.h"
#include "player.h"

void shared_systems::render_system(World& world, Graphic& graphic) {
  auto query = world.query<SDL_Texture*, TexturePosition, RenderPosition>();
  for (auto [texture, src, dest] : query) {
    auto dest_rect = graphic.camera().get_position_for(dest).rect;
    SDL_RenderTexture(graphic.renderer(), texture, &src.rect, &dest_rect);
  }

  // render player shooting position
  for (auto [player_position, shoot_position] :
       world.query<RenderPosition, player::ShootPosition>()) {
    if (shoot_position.show) {
      auto player_dest_position =
          graphic.camera().get_position_for(player_position);
      auto px = player_dest_position.rect.x + player_dest_position.rect.w / 2;
      auto py = player_dest_position.rect.y + player_dest_position.rect.h / 2;
      SDL_RenderLine(graphic.renderer(), px, py, shoot_position.x,
                     shoot_position.y);
    }
  }
};
