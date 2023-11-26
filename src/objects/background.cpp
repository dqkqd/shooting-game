#include "objects/background.h"

#include "components/primitive.h"
#include "components/texture.h"
#include "ecs/world.h"

void BackGround::init(const Graphic& graphic, World& world) {
  top_left_grass_ = TextureManager::add_from_file_unchecked(
      graphic.renderer(), texture_file::TOP_LEFT_GRASS);
  top_right_grass_ = TextureManager::add_from_file_unchecked(
      graphic.renderer(), texture_file::TOP_RIGHT_GRASS);
  top_middle_grass_ = TextureManager::add_from_file_unchecked(
      graphic.renderer(), texture_file::TOP_MIDDLE_GRASS);

  auto left_size = get_texture_size(top_left_grass_);
  auto right_size = get_texture_size(top_right_grass_);
  auto middle_size = get_texture_size(top_middle_grass_);

  const auto Y = static_cast<float>(graphic.height()) -
                 std::max({left_size.h, right_size.h, middle_size.h});

  const float VY = -5.0F;

  world.spawn_entity_with(std::move(top_left_grass_),
                          Position{0, Y, left_size.w, left_size.h},
                          Speed{0, -VY});
  world.spawn_entity_with(
      std::move(top_right_grass_),
      Position{static_cast<float>(graphic.width()) - right_size.w, Y,
               right_size.w, right_size.h},
      Speed{0, -VY});

  auto x = left_size.w;
  while (x + right_size.w <= static_cast<float>(graphic.width())) {
    world.spawn_entity_with(std::move(top_middle_grass_),
                            Position{x, Y, middle_size.w, middle_size.h},
                            Speed{0, -VY});
    x += middle_size.w;
  }

  std::function<void(Query<Position, Speed>)> moving_system =
      [Y](Query<Position, Speed> query) {
        for (auto [position, speed] : query) {
          position.rect.x += speed.vx;
          position.rect.y += speed.vy;

          if (position.rect.y <= 0) {
            position.rect.y = 0;
            speed.vy = -speed.vy;
          } else if (position.rect.y >= Y) {
            position.rect.y = Y;
            speed.vy = -speed.vy;
          }
        }
      };

  world.add_system(std::move(moving_system));
}
