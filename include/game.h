#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "ecs/systems/core.h"
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

  template <typename Event>
  void run(World& world, sys::SystemManager<Event> event_systems,
           sys::SystemManager<> normal_systems);

  void run_test_leak(World& world);

 private:
  Graphic graphic_;
};

template <typename Event>
void Game::run(World& world, sys::SystemManager<Event> event_systems,
               sys::SystemManager<> normal_systems) {
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0U) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
      event_systems.run(world, e);
    }

    SDL_SetRenderDrawColor(graphic_.renderer(), 0, 0, 0, 255);
    SDL_RenderClear(graphic_.renderer());

    normal_systems.run(world);

    SDL_RenderPresent(graphic_.renderer());
  }
}
#endif
