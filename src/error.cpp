#include "error.h"

#include "SDL3_image/SDL_image.h"
#include "SDL_error.h"

GameError::GameError(const char* msg) : error_msg_{msg} {}

auto GameError::from_sdl(const char* msg) -> GameError {
  std::ostringstream error_msg;
  error_msg << msg << ' ' << SDL_GetError() << std::endl;
  return GameError(error_msg.str().c_str());
}

auto GameError::from_sdl_img(const char* msg) -> GameError {
  std::ostringstream error_msg;
  error_msg << msg << ' ' << IMG_GetError() << std::endl;
  return GameError(error_msg.str().c_str());
}

auto GameError::what() const noexcept -> const char* {
  return error_msg_.c_str();
}