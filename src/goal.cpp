#include "goal.h"

#include "config.h"
#include "utils.h"

void Goal::init(World& world, Graphic& graphic) {
  auto config = GameConfig::data().goal;
  auto [texture, texture_info, render_info, animation] =
      utils::load_sprite(config, graphic);

  render_info.rect.x = static_cast<float>(config.position.x);
  render_info.rect.y = static_cast<float>(config.position.y);
  world.spawn_entity_with(std::move(texture), std::move(texture_info),
                          std::move(render_info), std::move(animation),
                          GoalInfo());
}
