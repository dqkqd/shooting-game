#ifndef COMPONENTS_GRAPHIC_H
#define COMPONENTS_GRAPHIC_H

#include <memory>

#include "SDL_render.h"

class Graphic;

class GraphicComponent {
 public:
  GraphicComponent(SDL_Texture* texture, SDL_FRect position);

  GraphicComponent(const GraphicComponent&) = delete;
  GraphicComponent(GraphicComponent&&) = delete;
  auto operator=(const GraphicComponent&) -> GraphicComponent& = delete;
  auto operator=(GraphicComponent&&) -> GraphicComponent& = delete;

  ~GraphicComponent();

  static auto from_file(Graphic& graphic, const char* filename)
      -> std::shared_ptr<GraphicComponent>;

  void update(Graphic& graphic);

  auto get_texture() -> SDL_Texture*;
  auto get_position() -> SDL_FRect&;

 private:
  SDL_Texture* texture_;
  SDL_FRect position_;
};

#endif
