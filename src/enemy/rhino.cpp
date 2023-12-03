#include "enemy/rhino.h"

#include <asm-generic/errno.h>

#include "components/animation.h"
#include "config.h"
#include "services/texture.h"

void Rhino::init(World& world, Graphic& graphic) {
  auto& rhino = GameConfig::data().enemy.rhino;

  auto* texture = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                          rhino.image.c_str());
  auto texture_size = get_texture_size(texture);

  auto width = texture_size.w / static_cast<float>(rhino.total_sprites);
  auto height = texture_size.h;

  auto animation =
      TextureAnimation(rhino.frames_delay, width, height, rhino.total_sprites);
  auto texture_position = animation.next_position();

  for (const auto& position : rhino.positions) {
    auto start_position =
        RenderPosition{static_cast<float>(position.x),
                       static_cast<float>(position.y), width, height};
    world.spawn_entity_with(std::move(texture), std::move(texture_position),
                            std::move(start_position), std::move(animation));
  }
}
