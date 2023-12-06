#ifndef COMMON_H
#define COMMON_H

#include "components/animation.h"
#include "config.h"
#include "graphic.h"

namespace game_common {

auto load_sprite(const config::Sprite& config, Graphic& graphic)
    -> std::tuple<SDL_Texture*, TextureInfo, RenderInfo, TextureAnimation>;

};  // namespace game_common

#endif
