#include "goal.h"

#include "config.h"
#include "game_state.h"
#include "player/player.h"
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

auto collide_with_player(World& world) -> bool {
  for (auto [player_render_info, player_info] :
       world.query<RenderInfo, PlayerInfo>()) {
    for (auto [goal_render_info, goal_info] :
         world.query<RenderInfo, GoalInfo>()) {
      if (player_render_info.collide(goal_render_info)) {
        return true;
      }
    }
  }
  return false;
}

void Goal::check_win_system(World& world) {
  if (collide_with_player(world)) {
    for (auto [game_info] : world.query<GameInfo>()) {
      game_info.status = GameStatus::FINISH;
    }
  }
}
