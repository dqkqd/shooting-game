#include "utils.h"

#include "config.h"
#include "graphic.h"
#include "services/texture.h"

auto game_common::load_sprite(const config::Sprite& config, Graphic& graphic)
    -> std::tuple<SDL_Texture*, TexturePosition, RenderPosition,
                  TextureAnimation> {
  auto* texture = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                          config.image.c_str());
  auto size = get_texture_size(texture);

  auto width = size.w / static_cast<float>(config.total_sprites);
  auto height = size.h;

  auto animation = TextureAnimation(config.frames_delay, width, height,
                                    config.total_sprites);
  auto texture_position = animation.next_position({});

  auto start_position = RenderPosition{0, 0, width, height};

  return std::make_tuple(std::move(texture), std::move(texture_position),
                         std::move(start_position), std::move(animation));
}
