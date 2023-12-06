#include "enemy/rhino.h"

#include <asm-generic/errno.h>

#include "SDL_render.h"
#include "components/position.h"
#include "config.h"
#include "utils.h"

void Rhino::init(World& world, Graphic& graphic) {
  auto& config = GameConfig::data().enemy.rhino;

  auto [texture, texture_info, render_info_initial, animation] =
      utils::load_sprite(config, graphic);

  for (std::size_t i = 0; i < config.positions.size(); ++i) {
    auto position = config.positions[i];
    auto from = SDL_FPoint{static_cast<float>(config.from[i].x),
                           static_cast<float>(config.from[i].y)};
    auto to = SDL_FPoint{static_cast<float>(config.to[i].x),
                         static_cast<float>(config.to[i].y)};

    auto render_info = render_info_initial;
    render_info.rect.x = static_cast<float>(position.x);
    render_info.rect.y = static_cast<float>(position.y);

    world.spawn_entity_with(std::move(texture), std::move(texture_info),
                            std::move(render_info), std::move(animation),
                            RhinoInfo{config.speed, from, to});
  }
}

void Rhino::moving_system(World& world) {
  for (auto [info, texture_info, render_info] :
       world.query<RhinoInfo, TextureInfo, RenderInfo>()) {
    render_info.rect.x += info.speed;

    auto change_direction = false;

    if (render_info.rect.x > info.to.x) {
      render_info.rect.x = info.to.x;
      change_direction = true;
    } else if (render_info.rect.x < info.from.x) {
      render_info.rect.x = info.from.x;
      change_direction = true;
    }

    if (change_direction) {
      info.speed = -info.speed;
      if (info.speed < 0) {
        texture_info.flip = SDL_FLIP_NONE;
      } else {
        texture_info.flip = SDL_FLIP_HORIZONTAL;
      }
    }
  }
}
