#include "systems.h"

#include "components/animation.h"
#include "components/position.h"
#include "config.h"
#include "services/texture.h"

void shared_systems::init_guide(World& world, Graphic& graphic) {
  auto* texture = TextureManager::add_from_file_unchecked(
      graphic.renderer(), GameConfig::data().guide_image.c_str());
  auto size = get_texture_size(texture);
  auto texture_info = TextureInfo{{0, 0, size.w, size.h}};

  auto camera_config = GameConfig::data().camera;
  auto render_info =
      RenderInfo{(static_cast<float>(camera_config.width) - size.w) / 2.0F,
                 (static_cast<float>(camera_config.height) - size.h) / 2.0F,
                 size.w, size.h};
  world.spawn_entity_with(std::move(texture), std::move(texture_info),
                          std::move(render_info));
}

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

void shared_systems::animation_system(World& world) {
  for (auto [texture_info, query_animation] :
       world.query<TextureInfo, TextureAnimation>()) {
    texture_info = query_animation.next_render_info(std::move(texture_info));
  }
}
