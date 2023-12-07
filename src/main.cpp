
#include "config.h"
#include "ecs/system.h"
#include "enemy/rhino.h"
#include "game.h"
#include "game_state.h"
#include "goal.h"
#include "guide.h"
#include "player/bullet.h"
#include "player/player.h"
#include "player/shooter.h"
#include "systems.h"
#include "tiles/tilemap.h"

auto main() -> int {
  auto title = GameConfig::data().game_title;
  auto game = Game(std::move(title), GameConfig::data().graphic.width,
                   GameConfig::data().graphic.height);

  auto world = World();

  SystemManager<SDL_Event> event_systems;
  event_systems.add_sequential(Shooter::shoot_system, game.graphic().camera())
      .add_parallel(Shooter::assign_position_system, game.graphic().camera())
      .add_sequential(GameState::restart_game_system);

  SystemManager normal_systems;
  normal_systems.add_parallel(shared_systems::animation_system)
      .add_parallel(Player::moving_system)
      .add_parallel(Bullet::moving_system)
      .add_parallel(Rhino::moving_system)
      .add_sequential(GameState::check_game_over_system)
      .add_sequential(Player::camera_system, game.graphic().camera())
      .add_sequential(shared_systems::render_system, game.graphic())
      .add_sequential(Shooter::indicator_render_system, game.graphic())
      .add_sequential(GameState::render_game_state_system, game.graphic())
      .add_sequential(Rhino::clear_dead_system)
      .add_sequential(Goal::check_win_system);

  TileMap tile_map(GameConfig::data().tile_map.background.c_str());
  tile_map.init(world, game.graphic());

  Player::init(world, game.graphic());
  Player::init_dead_player(world, game.graphic());
  Shooter::init(world, game.graphic());
  Bullet::init(world, game.graphic());

  Rhino::init(world, game.graphic());
  Rhino::init_dead_rhino(world, game.graphic());

  GameState::init(world, game.graphic());

  Goal::init(world, game.graphic());

  Guide::init(world, game.graphic());

  game.run(world, event_systems, normal_systems);

  return 0;
}
