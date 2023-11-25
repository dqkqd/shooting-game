
#include "components/primitive.h"
#include "game.h"
#include "objects/background.h"

auto main() -> int {
  auto game = Game("Hello", 800, 600);

  std::function<void(Query<SDL_Texture*, Position>)> render_system =
      [&game](Query<SDL_Texture*, Position> query) {
        for (auto [texture, position] : query) {
          SDL_RenderTexture(game.graphic().renderer(), texture, NULL,
                            &position.rect);
        }
      };

  auto world = World();
  world.add_system(std::move(render_system));

  BackGround bg(game.graphic().renderer());
  bg.setup(world);

  game.run(world);

  return 0;
}
