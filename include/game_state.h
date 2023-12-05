#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "components/position.h"
#include "ecs/world.h"
#include "graphic.h"

enum class GameStatus {
  PLAYING,
  GAME_OVER,
};

struct GameInfo {
  SDL_Texture* texture = NULL;
  TexturePosition src;
  RenderPosition dest{};
  GameStatus status = GameStatus::PLAYING;
};

class GameState {
 public:
  static void init(World& world, Graphic& graphic);

  static void game_over_system(World& world);
  static void game_over_render_system(World& world, Graphic& graphic);
};

#endif
