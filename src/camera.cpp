#include "camera.h"

#include "components/position.h"
#include "config.h"

Camera::Camera(int width, int height)
    : pos_{0, 0, static_cast<float>(width), static_cast<float>(height)} {}

void Camera::center_to(const RenderInfo& info) {
  pos_.x = info.rect.x + info.rect.w / 2.0F - pos_.w / 2.0F;
  pos_.x =
      std::clamp(pos_.x, 0.0F,
                 static_cast<float>(GameConfig::data().level.width) - pos_.w);
}

auto Camera::get_render_info_for(const RenderInfo& info) const -> RenderInfo {
  return {info.rect.x - pos_.x, info.rect.y, info.rect.w, info.rect.h};
}
