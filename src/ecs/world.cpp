#include "ecs/world.h"

Archetypes World::archetypes_{};
std::unordered_map<EntityId, EntityLocation> World::entities_{};
