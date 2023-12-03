#include "camera.h"

#include "components/position.h"
#include "config.h"

Camera::Camera(int width, int height)
    : pos_{0, 0, static_cast<float>(width), static_cast<float>(height)} {}

void Camera::center_to(const RenderPosition& position) {
  pos_.x = position.rect.x + position.rect.w / 2.0F - pos_.w / 2.0F;
  pos_.y = position.rect.y + position.rect.h / 2.0F - pos_.h / 2.0F;

  pos_.x =
      std::clamp(pos_.x, 0.0F,
                 static_cast<float>(GameConfig::data().level.width) - pos_.w);
  pos_.y =
      std::clamp(pos_.y, 0.0F,
                 static_cast<float>(GameConfig::data().level.height) - pos_.h);
}

auto Camera::get_position_for(const RenderPosition& position) const
    -> RenderPosition {
  return {position.rect.x - pos_.x, position.rect.y - pos_.y, position.rect.w,
          position.rect.h};
}
