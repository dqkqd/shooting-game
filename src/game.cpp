#include "game.h"

#include "SDL3_image/SDL_image.h"
#include "SDL_init.h"
#include "SDL_log.h"
#include "services/texture.h"

GameBase::GameBase() { assert(init()); }

GameBase::~GameBase() {
  TextureManager::clear();
  IMG_Quit();
  SDL_Quit();
}

auto GameBase::init() -> bool {
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

Game::Game(std::string&& title, int width, int height)
    : graphic_{std::move(title), width, height} {}

auto Game::graphic() -> Graphic& { return graphic_; }

void Game::run(World& world) {
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

void Game::run_test_leak(World& world) {
  for (int i = 0; i < 10000; ++i) {
    SDL_SetRenderDrawColor(graphic().renderer(), 0, 0, 0, 255);
    SDL_RenderClear(graphic().renderer());

    world.run_systems();

    SDL_RenderPresent(graphic().renderer());
  }
}
