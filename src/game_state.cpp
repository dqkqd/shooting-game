#include "game_state.h"

#include "SDL_events.h"
#include "components/physics.h"
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

  auto texture_position =
      TexturePosition{{0, 0, texture_size.w, texture_size.h}};
  auto render_position = RenderPosition{
      (static_cast<float>(camera_config.width) - texture_size.w) / 2.0F,
      (static_cast<float>(camera_config.height) - texture_size.h) / 2.0F,
      texture_size.w, texture_size.h};
  world.spawn_entity_with(GameInfo{
      .texture = std::move(texture),
      .src = std::move(texture_position),
      .dest = std::move(render_position),
      .status = GameStatus::PLAYING,
  });
}

void GameState::game_over_system(World& world) {
  for (auto [game_info] : world.query<GameInfo>()) {
    if (game_info.status == GameStatus::GAME_OVER) {
      continue;
    }
    for (auto [src_position, dest_position, player_info] :
         world.query<TexturePosition, RenderPosition, PlayerInfo>()) {
      if (Player::should_dead(world, dest_position)) {
        game_info.status = GameStatus::GAME_OVER;
        src_position.hidden = true;
        Player::make_player_dead(world, dest_position, player_info);
      }
    }
  }
}

void GameState::game_over_render_system(World& world, Graphic& graphic) {
  for (auto [info] : world.query<GameInfo>()) {
    if (info.status == GameStatus::GAME_OVER) {
      SDL_RenderTexture(graphic.renderer(), info.texture, &info.src.rect,
                        &info.dest.rect);
    }
  }
}

void GameState::game_restart_system(World& world, SDL_Event event) {
  if (event.type == SDL_EVENT_KEY_DOWN) {
    for (auto [info] : world.query<GameInfo>()) {
      if (info.status == GameStatus::GAME_OVER) {
        info.status = GameStatus::PLAYING;
        Player::restart_player(world);
      }
    }
  }
}