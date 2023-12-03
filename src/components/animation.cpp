#include "components/animation.h"

TextureAnimation::TextureAnimation(int frame_delay,  // NOLINT
                                   float sprite_width, float sprite_height,
                                   int total_sprites)
    : frame_delays_(frame_delay),
      sprite_width_(sprite_width),
      sprite_height_(sprite_height),
      total_sprites_(total_sprites) {}

auto TextureAnimation::next_position(const TexturePosition& position)
    -> TexturePosition {
  ++current_sprite_;

  auto x = (current_sprite_ / frame_delays_) % total_sprites_;
  return TexturePosition{
      {static_cast<float>(x) * sprite_width_, 0,
       static_cast<float>(sprite_width_), static_cast<float>(sprite_height_)},
      position.flip};
}
