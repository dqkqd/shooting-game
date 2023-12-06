#include "player/bullet.h"

#include "components/animation.h"
#include "components/physics.h"
#include "config.h"
#include "services/texture.h"

void Bullet::init(World& world, Graphic& graphic) {
  auto config = GameConfig::data().player.bullet;

  auto* texture = TextureManager::add_from_file_unchecked(graphic.renderer(),
                                                          config.image.c_str());
  auto texture_size = get_texture_size(texture);

  auto width = texture_size.w / static_cast<float>(config.total_sprites);
  auto height = texture_size.h;

  auto animation = TextureAnimation(config.frames_delay, width, height,
                                    config.total_sprites);
  auto texture_position = animation.next_position({.hidden = true});

  auto start_position = RenderPosition{0, 0, width, height};

  world.spawn_entity_with(std::move(texture), std::move(texture_position),
                          std::move(start_position), std::move(animation),
                          ProjectileMotion(0, 0), BulletInfo());
}

void Bullet::moving_system(World& world) {
  for (auto [info, position, motion] :
       world.query<BulletInfo, RenderPosition, ProjectileMotion>()) {
    if (info.status == BulletStatus::HIDDEN) {
      continue;
    }
    auto offset = motion.next_offset();
    position.rect.x += offset.dx;
    position.rect.y += offset.dy;
  }
};
