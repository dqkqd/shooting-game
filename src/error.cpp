#include "error.h"

#include <spdlog/fmt/fmt.h>

#include "SDL3_image/SDL_image.h"
#include "SDL_error.h"

GameError::GameError(const char* msg) : error_msg_{msg} {}

auto GameError::from_sdl(const char* msg) -> GameError {
  return GameError(fmt::format("{}, {}", msg, SDL_GetError()).c_str());
}

auto GameError::from_sdl_img(const char* msg) -> GameError {
  return GameError(fmt::format("{}, {}", msg, IMG_GetError()).c_str());
}

auto GameError::what() const noexcept -> const char* {
  return error_msg_.c_str();
}
