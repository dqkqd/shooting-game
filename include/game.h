#include <SDL3/SDL.h>

#include <string>

class Game {
 public:
  Game();
  ~Game();
  Game(const Game &) = default;
  Game(Game &&) = delete;
  auto operator=(const Game &) -> Game & = default;
  auto operator=(Game &&) -> Game & = delete;

  void run();

 private:
  SDL_Window *window_;

  static constexpr std::string SCREEN_TITLE = "Hello game";
  static constexpr int SCREEN_WIDTH = 640;
  static constexpr int SCREEN_HEIGHT = 480;
};