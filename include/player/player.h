#ifndef PLAYER_PLAYER_H
#define PLAYER_PLAYER_H

#include "camera.h"
#include "components/position.h"
#include "ecs/world.h"
#include "graphic.h"

enum class PlayerStatus {
  IDLE,
  STOPPED,
  SELF_SHOOTING,
  DEAD,
};

struct PlayerDeadInfo {};

struct PlayerInfo {
  PlayerStatus status = PlayerStatus::IDLE;
};

struct Collision {
  bool x;
  bool y;
};

class Player {
 public:
  static void init(World& world, Graphic& graphic);
  static void init_dead_player(World& world, Graphic& graphic);

  static void animation_system(World& world);
  static void moving_system(World& world);
  static void camera_system(World& world, Camera& camera);

  static auto should_dead(World& world, RenderPosition& position) -> bool;
  static void make_player_dead(World& world, RenderPosition& position,
                               PlayerInfo& info);
  static void restart_player(World& world);
};

#endif
