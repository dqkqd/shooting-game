#ifndef ENTITY_H
#define ENTITY_H

#include <memory>

#include "SDL_render.h"
#include "graphic.h"

class GameEntity {
 public:
  explicit GameEntity(SDL_Texture* texture = NULL,
                      SDL_FRect position = {0, 0, 0, 0});
  GameEntity(const GameEntity&) = delete;
  GameEntity(GameEntity&&) = delete;
  auto operator=(const GameEntity&) -> GameEntity& = delete;
  auto operator=(GameEntity&&) -> GameEntity& = delete;
  ~GameEntity();

  void update(Graphic& graphic);

  static auto from_file(Graphic& graphic, const char* filename) -> GameEntity;

 private:
  SDL_Texture* texture_;
  SDL_FRect position_;
};

#endif