#include "enemy/rhino.h"

#include <asm-generic/errno.h>

#include "SDL_render.h"
#include "components/animation.h"
#include "components/position.h"
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
  auto texture_position = animation.next_position(TexturePosition{});

  for (std::size_t i = 0; i < rhino.positions.size(); ++i) {
    auto position = rhino.positions[i];
    auto from = Point{static_cast<float>(rhino.from[i].x),
                      static_cast<float>(rhino.from[i].y)};
    auto to = Point{static_cast<float>(rhino.to[i].x),
                    static_cast<float>(rhino.to[i].y)};

    auto start_position =
        RenderPosition{static_cast<float>(position.x),
                       static_cast<float>(position.y), width, height};

    world.spawn_entity_with(std::move(texture), std::move(texture_position),
                            std::move(start_position), std::move(animation),
                            RhinoInfo{rhino.speed, from, to});
  }
}

void Rhino::moving_system(World& world) {
  for (auto [info, texture_pos, position] :
       world.query<RhinoInfo, TexturePosition, RenderPosition>()) {
    position.rect.x += info.speed;

    auto change_direction = false;

    if (position.rect.x > info.to.x) {
      position.rect.x = info.to.x;
      change_direction = true;
    } else if (position.rect.x < info.from.x) {
      position.rect.x = info.from.x;
      change_direction = true;
    }

    if (change_direction) {
      info.speed = -info.speed;
      if (info.speed < 0) {
        texture_pos.flip = SDL_FLIP_NONE;
      } else {
        texture_pos.flip = SDL_FLIP_HORIZONTAL;
      }
    }
  }
}
