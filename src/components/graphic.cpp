#include "components/graphic.h"

#include <SDL_render.h>
#include <error.h>

#include "graphic.h"

GraphicComponent::GraphicComponent(SDL_Texture* texture, SDL_FRect position)
    : texture_{texture}, position_{position} {}

GraphicComponent::~GraphicComponent() { SDL_DestroyTexture(texture_); }

auto GraphicComponent::from_file(Graphic& graphic, const char* filename)
    -> std::shared_ptr<GraphicComponent> {
  auto* texture = graphic.load_texture(filename);

  int texture_width = 0;
  int texture_height = 0;
  if (SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height) <
      0) {
    throw GameError::from_sdl(
        "Can not query texture when loading GraphicComponent from file");
  }

  SDL_FRect position{0, 0, static_cast<float>(texture_width),
                     static_cast<float>(texture_height)};

  return std::make_shared<GraphicComponent>(texture, position);
}

void GraphicComponent::update(Graphic& graphic) { graphic.render(*this); }

auto GraphicComponent::get_texture() -> SDL_Texture* { return texture_; }

auto GraphicComponent::get_position() -> SDL_FRect& { return position_; }