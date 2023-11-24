
#include "SDL3_image/SDL_image.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "game.h"

auto main() -> int {
  auto game = Game("Hello", 800, 600);
  game.init();

  auto world = World();

  world.spawn_entity_with(IMG_LoadTexture(game.renderer(), "assets/ground.png"),
                          SDL_FRect{0, 500, 800, 100});

  std::function<void(Query<SDL_Texture*, SDL_FRect>)> render_system =
      [&game](Query<SDL_Texture*, SDL_FRect> query) {
        for (auto [texture, position] : query) {
          SDL_RenderTexture(game.renderer(), texture, NULL, &position);
        }
      };

  auto velocity = -5.0F;

  std::function<void(Query<SDL_FRect>)> moving_system =
      [&velocity](Query<SDL_FRect> query) {
        for (auto [texture] : query) {
          texture.y += velocity;

          if (texture.y < 0) {
            texture.y = 0;
            velocity *= -1;
          } else if (texture.y > 500) {
            texture.y = 500;
            velocity *= -1;
          }
        }
      };

  world.add_system(moving_system);
  world.add_system(render_system);

  game.run(world);

  return 0;
}
