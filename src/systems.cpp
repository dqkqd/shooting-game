#include "systems.h"

void shared_systems::render_system(World& world, Graphic& graphic) {
  auto query = world.query<SDL_Texture*, TexturePosition, RenderPosition>();
  for (auto [texture, src, dest] : query) {
    auto dest_rect = graphic.camera().get_position_for(dest).rect;
    SDL_RenderTexture(graphic.renderer(), texture, &src.rect, &dest_rect);
  }
};
