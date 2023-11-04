#ifndef ENTITY_H
#define ENTITY_H

#include <memory>

#include "SDL_render.h"
#include "components/graphic.h"
#include "graphic.h"

class GameEntity {
 public:
  void update(Graphic& graphic);

  // TODO(khanhdq): we might need unique_ptr instead of shared ptr
  static auto create_entity(
      const std::shared_ptr<GraphicComponent>& graphic_component) -> GameEntity;
  static auto from_file(Graphic& graphic, const char* filename) -> GameEntity;

 private:
  std::shared_ptr<GraphicComponent> graphic_component_;

  explicit GameEntity(
      const std::shared_ptr<GraphicComponent>& graphic_component);
};

#endif
