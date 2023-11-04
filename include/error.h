#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <iomanip>
#include <stdexcept>

class GameError : public std::exception {
 public:
  explicit GameError(const char* msg);
  static auto from_sdl(const char* msg) -> GameError;
  static auto from_sdl_img(const char* msg) -> GameError;
  [[nodiscard]] auto what() const noexcept -> const char* override;

 private:
  std::string error_msg_;
};
#endif
