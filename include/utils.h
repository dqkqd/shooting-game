#ifndef COMMON_H
#define COMMON_H

#include "components/animation.h"
#include "config.h"
#include "ecs/world.h"
#include "graphic.h"

namespace utils {

auto load_sprite(const config::Sprite& config, Graphic& graphic)
    -> std::tuple<SDL_Texture*, TextureInfo, RenderInfo, TextureAnimation>;

auto out_of_game_screen(RenderInfo& info) -> bool;
auto collide_with_rhino(World& world, RenderInfo& render_info) -> bool;

};  // namespace utils

#endif
