#ifndef GAME_H
#define GAME_H

#include "SDL3_image/SDL_image.h"
#include "SDL_init.h"
#include "SDL_log.h"
#include "ecs/world.h"
#include "graphic.h"

class GameBase {
 public:
  GameBase() { assert(init()); }

  ~GameBase() { quit(); }

  GameBase(const GameBase&) = delete;
  GameBase(GameBase&&) = delete;
  auto operator=(const GameBase&) -> GameBase& = delete;
  auto operator=(GameBase&&) -> GameBase& = delete;

 private:
  static auto init() -> bool {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) {
      SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
      return false;
    }

    auto img_flags = IMG_InitFlags::IMG_INIT_PNG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
      SDL_Log("IMG_Init failed: %s\n", SDL_GetError());
      return false;
    };

    return true;
  }

  static void quit() {
    IMG_Quit();
    SDL_Quit();
  };
};

class Game : public GameBase {
 public:
  explicit Game(std::string&& title, int width, int height)  // NOLINT
      : graphic_{std::forward<std::string>(title), width, height} {}

  void run(World& world) {
    SDL_Event e;
    bool quit = false;
    while (!quit) {
      while (SDL_PollEvent(&e) != 0U) {
        if (e.type == SDL_EVENT_QUIT) {
          quit = true;
        }
      }

      SDL_SetRenderDrawColor(graphic_.renderer(), 0, 0, 0, 255);
      SDL_RenderClear(graphic_.renderer());

      world.run_systems();

      SDL_RenderPresent(graphic_.renderer());
    }
  }

  auto graphic() -> Graphic& { return graphic_; }

  void run_test_leak(World& world) {
    for (int i = 0; i < 10000; ++i) {
      SDL_SetRenderDrawColor(graphic().renderer(), 0, 0, 0, 255);
      SDL_RenderClear(graphic().renderer());

      world.run_systems();

      SDL_RenderPresent(graphic().renderer());
    }
  }

 private:
  Graphic graphic_;
};

#endif
