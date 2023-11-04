#include "game.h"

#include <iostream>
#include <memory>

#include "SDL3_image/SDL_image.h"
#include "SDL_init.h"
#include "entity.h"
#include "graphic.h"

Game::Game(std::string title) : title_{std::move(title)} { Game::init(); }

Game::~Game() { Game::quit(); }

void Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    // TODO(khanhdq): throw error
  }

  if (IMG_Init(IMG_InitFlags::IMG_INIT_PNG) == 0) {
    // TODO(khanhdq): throw error
  }
}

void Game::quit() {
  IMG_Quit();
  SDL_Quit();
}

auto Game::create_graphic() -> std::unique_ptr<Graphic> {
  return std::make_unique<Graphic>(SCREEN_WIDTH, SCREEN_HEIGHT, title_.c_str());
}

void Game::run() {
  // TODO(khanhdq): put this into thread
  auto graphic = create_graphic();
  auto background = GameEntity::from_file(*graphic, "assets/ground.png");

  SDL_Event event;

  constexpr int DELAY = 100;
  constexpr int RED = 255;
  constexpr int GREEN = 255;
  constexpr int BLUE = 255;

  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    background.update(*graphic);
    graphic->update();

    SDL_Delay(DELAY);
  }
}
