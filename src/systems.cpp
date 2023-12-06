#include "systems.h"

#include "components/position.h"

void shared_systems::render_system(World& world, Graphic& graphic) {
  for (auto [texture, src, dest] :
       world.query<SDL_Texture*, TextureInfo, RenderInfo>()) {
    if (src.hidden) {
      continue;
    }

    auto dest_rect = graphic.camera().get_render_info_for(dest).rect;
    SDL_RenderTextureRotated(graphic.renderer(), texture, &src.rect, &dest_rect,
                             0, NULL, src.flip);
  }
};
