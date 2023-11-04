#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>

#include <memory>
#include <string>

#include "graphic.h"

class Game {
 public:
  explicit Game(std::string title);
  ~Game();
  Game(const Game &) = delete;
  Game(Game &&) = delete;
  auto operator=(const Game &) -> Game & = delete;
  auto operator=(Game &&) -> Game & = delete;

  void run();

 private:
  // explicitly init and quit SDL systems
  static void init();
  static void quit();

  auto create_graphic() -> std::unique_ptr<Graphic>;

  std::string title_;

  static constexpr int SCREEN_WIDTH = 640;
  static constexpr int SCREEN_HEIGHT = 480;
};

#endif