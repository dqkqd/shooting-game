#pragma once

#include "ecs/world.h"

#include "ecs/query/query.h"

template <typename T, typename... Args>
auto World::spawn_entity_with(T&& component, Args&&... components)
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
auto World::add_component_to_entity(EntityId entity_id, T&& component)
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

  auto new_location =
      archetypes_.get_by_id_unchecked(archetype_id)
          .move_entity_to_other(
              entity_id, archetypes_.get_by_id_unchecked(next_archetype_id),
              std::forward<T>(component));
  return new_location;
}

template <typename T>
auto World::remove_component_from_entity(EntityId entity_id)
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

  auto new_location =
      archetypes_.get_by_id_unchecked(archetype_id)
          .move_entity_to_other(
              entity_id, archetypes_.get_by_id_unchecked(prev_archetype_id));
  return new_location;
}

template <typename... Args>
void World::add_query() {
  queries_.emplace_back(std::make_unique<Query<Args...>>(*this));
}

template <typename... Args>
auto World::run_query(size_t index) -> std::tuple<Args&...> {
  auto& query = dynamic_cast<Query<Args...>&>(*queries_[index]);
  return query.next();
}
