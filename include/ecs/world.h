#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include <optional>

#include "ecs/archetype/archetypes.h"
#include "ecs/entity.h"
#include "ecs/query/query.h"

class World {
 public:
  World() = default;
  World(const World&) = delete;
  World(World&&) = delete;
  auto operator=(const World&) -> World& = delete;
  auto operator=(World&&) -> World& = delete;
  ~World() = default;

  template <typename T, typename... Args>
  auto spawn_entity_with(T&& component, Args&&... components) -> EntityLocation;

  template <typename T>
  auto add_component_to_entity(EntityId entity_id, T&& component)
      -> std::optional<EntityLocation>;

  template <typename T>
  auto remove_component_from_entity(EntityId entity_id)
      -> std::optional<EntityLocation>;

  auto archetypes() -> Archetypes& { return archetypes_; }

  template <typename... Args>
  void add_system(void (*system)(Query<Args...>));
  void run_systems();

 private:
  using System = std::function<void()>;

  Archetypes archetypes_;
  std::map<Components, QueryWrapper> queries_;
  std::unordered_map<EntityId, EntityLocation> entities_;

  std::vector<System> systems_;

  template <typename... Args>
  void add_query();
  template <typename... Args>
  auto get_query() -> Query<Args...>;
};

/* put the definition here since these are template methods */

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
  auto components = Components::from_types<Args...>();
  if (queries_.count(components) > 0) {
    return;
  }
  queries_.emplace(std::move(components),
                   QueryWrapper(archetypes_, archetypes_.find<Args...>()));
}

template <typename... Args>
auto World::get_query() -> Query<Args...> {
  return queries_.at(Components::from_types<Args...>())
      .template query<Args...>();
}

template <typename... Args>
void World::add_system(void (*system)(Query<Args...>)) {
  // setup to avoid re-calculating during game loop
  add_query<Args...>();
  systems_.emplace_back([=]() { system(get_query<Args...>()); });
}

inline void World::run_systems() {
  for (auto& system : systems_) {
    system();
  }
}

#endif
