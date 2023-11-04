#include "entity.h"

#include "SDL_render.h"
#include "components/graphic.h"

GameEntity::GameEntity(
    const std::shared_ptr<GraphicComponent>& graphic_component)
    : graphic_component_{graphic_component} {}

void GameEntity::update(Graphic& graphic) {
  graphic_component_->update(graphic);
}

auto GameEntity::create_entity(
    const std::shared_ptr<GraphicComponent>& graphic_component) -> GameEntity {
  return GameEntity(graphic_component);
}

auto GameEntity::from_file(Graphic& graphic, const char* filename)
    -> GameEntity {
  return GameEntity(GraphicComponent::from_file(graphic, filename));
}
