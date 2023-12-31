#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "components/position.h"
#include "ecs/world.h"
#include "graphic.h"

enum class GameStatus {
  PLAYING,
  GAME_OVER,
  FINISH,
};

struct GameStateSpriteInfo {
  SDL_Texture* texture = NULL;
  TextureInfo texture_info;
  RenderInfo render_info{};
};

struct GameInfo {
  GameStateSpriteInfo game_over;
  GameStateSpriteInfo game_finish;

  GameStatus status = GameStatus::PLAYING;
  std::chrono::time_point<std::chrono::system_clock> timestamp;
};

class GameState {
 public:
  static void init(World& world, Graphic& graphic);

  static void check_game_over_system(World& world);
  static void render_game_state_system(World& world, Graphic& graphic);
  static void restart_game_system(World& world, SDL_Event event);
};

#endif
