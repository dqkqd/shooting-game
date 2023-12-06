#include "player/bullet.h"

#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "enemy/rhino.h"
#include "utils.h"

void Bullet::init(World& world, Graphic& graphic) {
  auto config = GameConfig::data().player.bullet;
  auto [texture, texture_info, render_info, animation] =
      utils::load_sprite(config, graphic);

  texture_info.hidden = true;
  world.spawn_entity_with(std::move(texture), std::move(texture_info),
                          std::move(render_info), std::move(animation),
                          ProjectileMotion(0, 0), BulletInfo());
}

void make_bullet_disappear(TextureInfo& texture_info, BulletInfo& info) {
  texture_info.hidden = true;
  info.status = BulletStatus::HIDDEN;
}

auto collide_with_tiles(World& world, RenderInfo& render_info) -> bool {
  for (auto [collidable_tile, _] :  // NOLINT
       world.query<RenderInfo, Collidable>()) {
    if (render_info.collide(collidable_tile)) {
      return true;
    }
  }
  return false;
}

void Bullet::moving_system(World& world) {
  for (auto [info, texture_info, render_info, motion] :
       world.query<BulletInfo, TextureInfo, RenderInfo, ProjectileMotion>()) {
    if (info.status == BulletStatus::HIDDEN) {
      continue;
    }
    auto offset = motion.next_offset();
    render_info.rect.x += offset.dx;
    render_info.rect.y += offset.dy;

    if (utils::out_of_game_screen(render_info) ||
        collide_with_tiles(world, render_info) ||
        Rhino::try_make_rhino_dead(world, render_info)) {
      make_bullet_disappear(texture_info, info);
    }
  }
};
