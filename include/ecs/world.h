#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "ecs/archetype/archetypes.h"
#include "ecs/entity.h"

class World {
 public:
  World() = default;

  template <typename T, typename... Args>
  auto spawn_entity_with(T&& component, Args&&... components)
      -> EntityLocation {
    auto entity_id = EntityCounter::id();
    auto archetype_id = archetypes_.get_or_add<T, Args...>();
    auto location = archetypes_.get_by_id_unchecked(archetype_id)
                        .add_entity(entity_id, std::forward<T>(component),
                                    std::forward<Args>(components)...);
    entities_.emplace(entity_id, location);
    return location;
  }

 private:
  Archetypes archetypes_;
  std::unordered_map<EntityId, EntityLocation> entities_;
};

#endif
