#include "entity.h"

#include "SDL_render.h"

GameEntity::GameEntity(SDL_Texture* texture, SDL_FRect position)
    : texture_{texture}, position_{position} {}

GameEntity::~GameEntity() { SDL_DestroyTexture(texture_); }

void GameEntity::update(Graphic& graphic) {
  // TODO(khanhdq): create GraphicComponent
  SDL_RenderTexture(graphic.renderer(), texture_, NULL, &position_);
}

auto GameEntity::from_file(Graphic& graphic, const char* filename)
    -> GameEntity {
  auto* texture = graphic.load_texture(filename);
  int texture_width = 0;
  int texture_height = 0;
  if (SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height) <
      0) {
    // TODO(khanhdq): throw error
  }
  return GameEntity(texture, {0, 0, static_cast<float>(texture_width),
                              static_cast<float>(texture_height)});
}
