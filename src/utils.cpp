#include "utils.h"

#include "config.h"
#include "enemy/rhino.h"
#include "graphic.h"
#include "services/texture.h"

auto utils::load_sprite(const config::Sprite& config, Graphic& graphic)
    -> std::tuple<SDL_Texture*, TextureInfo, RenderInfo, TextureAnimation> {
  auto* texture = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                          config.image.c_str());
  auto size = get_texture_size(texture);

  auto width = size.w / static_cast<float>(config.total_sprites);
  auto height = size.h;

  auto animation = TextureAnimation(config.frames_delay, width, height,
                                    config.total_sprites);
  auto texture_info = animation.next_render_info({});

  auto render_info = RenderInfo{0, 0, width, height};

  return std::make_tuple(std::move(texture), std::move(texture_info),
                         std::move(render_info), std::move(animation));
}

auto utils::out_of_game_screen(RenderInfo& info) -> bool {
  return info.rect.y > static_cast<float>(GameConfig::data().level.height);
}

auto utils::collide_with_rhino(World& world, RenderInfo& render_info) -> bool {
  for (auto [rhino_render_info, rhino_info] :  // NOLINT
       world.query<RenderInfo, RhinoInfo>()) {
    if (render_info.collide(rhino_render_info)) {
      return true;
    }
  }
  return false;
}
