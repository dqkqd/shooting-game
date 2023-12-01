#ifndef CAMERA_H
#define CAMERA_H

#include "SDL_rect.h"
#include "components/position.h"

class Camera {
 public:
  Camera(int width, int height);

  void center_to(const RenderPosition &position);
  [[nodiscard]] auto get_position_for(const RenderPosition &position) const
      -> RenderPosition;

 private:
  SDL_FRect pos_;
};

#endif
