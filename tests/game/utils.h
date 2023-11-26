#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <gtest/gtest.h>

#include "SDL_init.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "components/texture.h"
#include "game.h"

class SDLEnvironment : public ::testing::Environment {  // NOLINT
 public:
  ~SDLEnvironment() override {}

  GameBase game_base;

  // Override this to define how to set up the environment.
  void SetUp() override { SDL_Init(SDL_INIT_EVERYTHING); }

  static auto window() -> SDL_Window* {
    static auto* win = SDL_CreateWindow(NULL, 100, 100, SDL_WINDOW_HIDDEN);
    return win;
  }
  static auto renderer() -> SDL_Renderer* {
    static auto* ren =
        SDL_CreateRenderer(window(), NULL, SDL_RENDERER_PRESENTVSYNC);
    return ren;
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {
    TextureManager::clear();
    SDL_DestroyRenderer(renderer());
    SDL_DestroyWindow(window());
    SDL_Quit();
  }
};

#endif
