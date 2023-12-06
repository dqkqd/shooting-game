#ifndef CAMERA_H
#define CAMERA_H

#include "SDL_rect.h"
#include "components/position.h"

class Camera {
 public:
  Camera(int width, int height);

  void center_to(const RenderInfo &info);
  [[nodiscard]] auto get_render_info_for(const RenderInfo &info) const
      -> RenderInfo;

 private:
  SDL_FRect pos_;
};

#endif
