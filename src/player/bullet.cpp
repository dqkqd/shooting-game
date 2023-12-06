#include "player/bullet.h"

#include "components/physics.h"
#include "components/position.h"
#include "config.h"
#include "utils.h"

void Bullet::init(World& world, Graphic& graphic) {
  auto config = GameConfig::data().player.bullet;
  auto [texture, texture_position, start_position, animation] =
      game_common::load_sprite(config, graphic);

  texture_position.hidden = true;
  world.spawn_entity_with(std::move(texture), std::move(texture_position),
                          std::move(start_position), std::move(animation),
                          ProjectileMotion(0, 0), BulletInfo());
}

auto out_of_level(RenderPosition& position) -> bool {
  return position.rect.y > static_cast<float>(GameConfig::data().level.height);
}

void make_bullet_disappear(TexturePosition& position, BulletInfo& info) {
  position.hidden = true;
  info.status = BulletStatus::HIDDEN;
}

auto collide_with_tiles(World& world, RenderPosition& position) -> bool {
  for (auto [collidable_tile, _] :  // NOLINT
       world.query<RenderPosition, Collidable>()) {
    if (position.collide(collidable_tile)) {
      return true;
    }
  }
  return false;
}

void Bullet::moving_system(World& world) {
  for (auto [info, src_position, position, motion] :
       world.query<BulletInfo, TexturePosition, RenderPosition,
                   ProjectileMotion>()) {
    if (info.status == BulletStatus::HIDDEN) {
      continue;
    }
    auto offset = motion.next_offset();
    position.rect.x += offset.dx;
    position.rect.y += offset.dy;

    if (out_of_level(position) || collide_with_tiles(world, position)) {
      make_bullet_disappear(src_position, info);
    }
  }
};
