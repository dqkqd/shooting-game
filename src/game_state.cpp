#include "game_state.h"

#include "SDL_log.h"
#include "components/position.h"
#include "config.h"
#include "player/player.h"

void GameState::init(World& world) {
  world.spawn_entity_with(GameInfo{.status = GameStatus::PLAYING});
}

void GameState::game_over_system(World& world) {
  for (auto [position, _] : world.query<RenderPosition, PlayerInfo>()) {
    if (position.rect.y > static_cast<float>(GameConfig::data().level.height)) {
      for (auto [info] : world.query<GameInfo>()) {
        info.status = GameStatus::GAME_OVER;
      }
    }
  }
}

void GameState::game_over_render_system(World& world) {
  // render game over
  for (auto [info] : world.query<GameInfo>()) {
    if (info.status == GameStatus::GAME_OVER) {
      SDL_Log("GAME OVER");
    }
  }
}
