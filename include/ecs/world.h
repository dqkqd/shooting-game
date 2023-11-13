#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include <optional>

#include "ecs/archetype/archetype.h"
#include "ecs/archetype/archetypes.h"
#include "ecs/entity.h"

class World {
 public:
  World() = delete;

  template <typename T, typename... Args>
  static auto spawn_entity_with(T&& component, Args&&... components)
      -> EntityLocation {
    auto entity_id = EntityCounter::id();
    auto location = archetypes_.get_or_add<T, Args...>().add_entity(
        entity_id, std::forward<T>(component),
        std::forward<Args>(components)...);
    World::entities_.emplace(entity_id, location);
    return location;
  }

 private:
  static Archetypes archetypes_;
  static std::unordered_map<EntityId, EntityLocation> entities_;
};

#endif
