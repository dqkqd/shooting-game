#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "ecs/world.h"

enum class GameStatus {
  PLAYING,
  GAME_OVER,
};

struct GameInfo {
  GameStatus status;
};

class GameState {
 public:
  static void init(World& world);

  static void game_over_system(World& world);
};

#endif
