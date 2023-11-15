#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "ecs/archetype/archetypes.h"
#include "ecs/entity.h"

class World {
 public:
  World() = default;
  World(const World&) = delete;
  World(World&&) = delete;
  auto operator=(const World&) -> World& = delete;
  auto operator=(World&&) -> World& = delete;
  ~World() = default;

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

  template <typename T>
  auto add_component_to_entity(EntityId entity_id, T&& component)
      -> std::optional<EntityLocation> {
    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
      spdlog::warn("Could not add component {}: entity {} does not exist",
                   typeid(T).name(), entity_id);
      return {};
    }

    auto location = it->second;
    auto archetype_id = location.archetype_id;

    auto next_archetype_id =
        archetypes_.get_or_add_next_archetype<T>(archetype_id);
    if (!next_archetype_id.has_value()) {
      spdlog::warn(
          "Could not add component {}: component has already existed in "
          "archetype {}",
          typeid(T).name(), archetype_id);
      return {};
    }

    auto new_location =
        archetypes_.get_by_id_unchecked(archetype_id)
            .move_entity_to_other(
                entity_id, archetypes_.get_by_id_unchecked(*next_archetype_id),
                std::forward<T>(component));
    return new_location;
  }

  template <typename T>
  auto remove_component_from_entity(EntityId entity_id)
      -> std::optional<EntityLocation> {
    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
      spdlog::warn("Could not remove component {}: entity {} does not exist",
                   typeid(T).name(), entity_id);
      return {};
    }

    auto location = it->second;
    auto archetype_id = location.archetype_id;

    auto prev_archetype_id =
        archetypes_.get_or_add_prev_archetype<T>(archetype_id);
    if (!prev_archetype_id.has_value()) {
      spdlog::warn(
          "Could not remove component {}: component does not exist in "
          "archetype {}",
          typeid(T).name(), archetype_id);
      return {};
    }

    auto new_location =
        archetypes_.get_by_id_unchecked(archetype_id)
            .move_entity_to_other(
                entity_id, archetypes_.get_by_id_unchecked(*prev_archetype_id));
    return new_location;
  }

  auto archetypes() -> Archetypes&;

 private:
  Archetypes archetypes_;
  std::unordered_map<EntityId, EntityLocation> entities_;
};

#endif
