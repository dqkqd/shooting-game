#include "game.h"

#include <iostream>

#include "SDL_error.h"
#include "SDL_init.h"
#include "SDL_video.h"

Game::Game() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("Could not initialize video %s", SDL_GetError());
  } else {
    window_ =
        SDL_CreateWindow(SCREEN_TITLE.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WindowFlags::SDL_WINDOW_OPENGL);
    if (window_ == NULL) {
      SDL_Log("Could not initialize window %s", SDL_GetError());
    }
  }
}

Game::~Game() {
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

void Game::run() {
  SDL_Event event;

  constexpr int DELAY = 100;
  constexpr int RED = 255;
  constexpr int GREEN = 255;
  constexpr int BLUE = 255;

  SDL_Surface *screen_surface = NULL;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    screen_surface = SDL_GetWindowSurface(window_);
    SDL_FillSurfaceRect(screen_surface, NULL,
                        SDL_MapRGB(screen_surface->format, RED, GREEN, BLUE));
    SDL_UpdateWindowSurface(window_);
    SDL_Delay(DELAY);
  }
}
