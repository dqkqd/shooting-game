#include "game_state.h"

#include <chrono>

#include "SDL_events.h"
#include "components/position.h"
#include "config.h"
#include "graphic.h"
#include "player/player.h"
#include "services/texture.h"

void GameState::init(World& world, Graphic& graphic) {
  auto* texture = TextureManager::add_from_file_unchecked(
      graphic.renderer(),
      GameConfig::data().game_state.game_over_image.c_str());
  auto texture_size = get_texture_size(texture);

  auto camera_config = GameConfig::data().camera;

  auto texture_info = TextureInfo{{0, 0, texture_size.w, texture_size.h}};
  auto render_info = RenderInfo{
      (static_cast<float>(camera_config.width) - texture_size.w) / 2.0F,
      (static_cast<float>(camera_config.height) - texture_size.h) / 2.0F,
      texture_size.w, texture_size.h};
  world.spawn_entity_with(GameInfo{
      .texture = std::move(texture),
      .texture_info = std::move(texture_info),
      .render_info = std::move(render_info),
      .status = GameStatus::PLAYING,
  });
}

void GameState::check_game_over_system(World& world) {
  for (auto [game_info] : world.query<GameInfo>()) {
    if (game_info.status == GameStatus::GAME_OVER) {
      continue;
    }
    if (Player::try_make_player_dead(world)) {
      game_info.status = GameStatus::GAME_OVER;
      game_info.game_over_timestamp = std::chrono::system_clock::now();
    }
  }
}

void GameState::render_game_over_state_system(World& world, Graphic& graphic) {
  for (auto [info] : world.query<GameInfo>()) {
    if (info.status == GameStatus::GAME_OVER) {
      SDL_RenderTexture(graphic.renderer(), info.texture,
                        &info.texture_info.rect, &info.render_info.rect);

      // turn off player dead animation
      auto now = std::chrono::system_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
          now - info.game_over_timestamp);
      if (duration.count() > GameConfig::data().player.dead_state.last) {
        for (auto [texture_info, _] :
             world.query<TextureInfo, PlayerDeadInfo>()) {
          texture_info.hidden = true;
        }
      }
    }
  }
}

void GameState::restart_game_system(World& world, SDL_Event event) {
  if (event.type == SDL_EVENT_KEY_DOWN) {
    for (auto [info] : world.query<GameInfo>()) {
      if (info.status == GameStatus::GAME_OVER) {
        info.status = GameStatus::PLAYING;
        Player::restart_player(world);
      }
    }
  }
}
