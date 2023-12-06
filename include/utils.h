#ifndef COMMON_H
#define COMMON_H

#include "components/animation.h"
#include "config.h"
#include "graphic.h"

namespace utils {

auto load_sprite(const config::Sprite& config, Graphic& graphic)
    -> std::tuple<SDL_Texture*, TextureInfo, RenderInfo, TextureAnimation>;

auto out_of_game_screen(RenderInfo& info) -> bool;

};  // namespace utils

#endif
