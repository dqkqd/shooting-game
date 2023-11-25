#ifndef GAME_H
#define GAME_H

#include <string>

#include "SDL3_image/SDL_image.h"
#include "SDL_events.h"
#include "SDL_init.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "ecs/world.h"

class Game {  // NOLINT
 public:
  explicit Game(std::string&& title, int width, int height)  // NOLINT
      : title_{std::move(title)}, width_(width), height_(height) {
    init();
  }
  ~Game() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    IMG_Quit();
    SDL_Quit();
  }

  auto init() -> bool {
    // TODO(khanhdq): move this out
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
      return false;
    }

    auto img_flags = IMG_InitFlags::IMG_INIT_PNG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
      SDL_Log("IMG_Init failed: %s\n", SDL_GetError());
      return false;
    };

    window_ =
        SDL_CreateWindow(title_.c_str(), width_, height_, SDL_WINDOW_OPENGL);
    renderer_ = SDL_CreateRenderer(
        window_, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    return true;
  }

  void run(World& world) {
    SDL_Event e;
    bool quit = false;
    while (!quit) {
      while (SDL_PollEvent(&e) != 0U) {
        if (e.type == SDL_EVENT_QUIT) {
          quit = true;
        }
      }

      SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
      SDL_RenderClear(renderer_);

      world.run_systems();

      SDL_RenderPresent(renderer_);
    }
  }

  auto window() -> SDL_Window* { return window_; }
  auto renderer() -> SDL_Renderer* { return renderer_; }

  void run_test_leak(World& world) {
    for (int i = 0; i < 10000; ++i) {
      SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
      SDL_RenderClear(renderer_);

      world.run_systems();

      SDL_RenderPresent(renderer_);
    }
  }

 private:
  std::string title_;
  int width_;
  int height_;

  SDL_Window* window_ = NULL;
  SDL_Renderer* renderer_ = NULL;
};

#endif
