#ifndef ENEMY_RHINO_H
#define ENEMY_RHINO_H

#include "ecs/world.h"
#include "graphic.h"

enum class RhinoStatus {
  ALIVE,
  DEAD,
};

struct RhinoDeadInfo {
  std::chrono::time_point<std::chrono::system_clock> last_dead;
};

struct RhinoInfo {
  float speed;
  SDL_FPoint from;
  SDL_FPoint to;
  RhinoStatus status;
};

class Rhino {
 public:
  static void init(World& world, Graphic& graphic);
  static void init_dead_rhino(World& world, Graphic& graphic);

  static void moving_system(World& world);
  static void clear_dead_system(World& world);
  static auto try_make_rhino_dead(World& world, RenderInfo& bullet_render_info)
      -> bool;
};

#endif
