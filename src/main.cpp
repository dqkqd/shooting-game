
#include "components/primitive.h"
#include "components/texture.h"
#include "game.h"

auto main() -> int {
  auto game = Game("Hello", 800, 600);

  auto velocity = -5.0F;
  std::function<void(Query<Position>)> moving_system =
      [&velocity](Query<Position> query) {
        for (auto [position] : query) {
          position.rect.y += velocity;

          if (position.rect.y < 0) {
            position.rect.y = 0;
            velocity *= -1;
          } else if (position.rect.y > 500) {
            position.rect.y = 500;
            velocity *= -1;
          }
        }
      };

  std::function<void(Query<Texture, Position>)> render_system =
      [&game](Query<Texture, Position> query) {
        for (auto [texture, position] : query) {
          SDL_RenderTexture(game.graphic().renderer(), texture.data(), NULL,
                            &position.rect);
        }
      };

  auto world = World();
  auto texture =
      Texture::from_file(game.graphic().renderer(), "assets/ground.png");
  assert(texture.has_value());
  world.spawn_entity_with(std::move(*texture), Position{0, 500, 800, 100});
  world.add_system(moving_system).add_system(render_system);

  game.run(world);

  return 0;
}
