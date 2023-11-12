#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include <unordered_map>

#include "ecs/archetype.h"

using EntityId = int;

class World {
  std::unordered_map<EntityId, ArchetypeRecord> entities_;
};

#endif
