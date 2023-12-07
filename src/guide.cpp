

#include "guide.h"

#include "config.h"
#include "services/texture.h"

void Guide::init(World& world, Graphic& graphic) {
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
