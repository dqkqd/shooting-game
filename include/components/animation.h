#ifndef COMPONENTS_ANIMATION_H
#define COMPONENTS_ANIMATION_H

#include "components/position.h"

class TextureAnimation {
 public:
  TextureAnimation(int frame_delay, float sprite_width, float sprite_height,
                   int total_sprites);

  auto next_position() -> TexturePosition;

 private:
  int frame_delays_;
  float sprite_width_;
  float sprite_height_;
  int total_sprites_;

  int current_sprite_{};
};

#endif
