#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "ecs/system.h"
#include "ecs/world.h"
#include "graphic.h"

class GameBase {
 public:
  GameBase();
  ~GameBase();

  GameBase(const GameBase&) = delete;
  GameBase(GameBase&&) = delete;
  auto operator=(const GameBase&) -> GameBase& = delete;
  auto operator=(GameBase&&) -> GameBase& = delete;

 private:
  static auto init() -> bool;
};

class Game : public GameBase {
 public:
  explicit Game(std::string&& title, int width, int height);  // NOLINT

  auto graphic() -> Graphic&;
  auto camera() -> Camera&;

  void run(World& world, SystemManager<SDL_Event> event_systems,
           SystemManager<> normal_systems);

  void run_test_leak(World& world);

 private:
  Graphic graphic_;
};

#endif
