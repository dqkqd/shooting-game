#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include <optional>

#include "ecs/archetype/archetypes.h"
#include "ecs/entity.h"
#include "ecs/query/query.h"

class World {
 public:
  World() { init(); }

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
  auto query() -> Queries& { return queries_; }

  /* query specific systems */
  template <typename... Args>
  auto add_system(void (*system)(Query<Args...>)) -> World&;
  template <typename... Args>
  auto add_system(std::function<void(Query<Args...>)>&& system) -> World&;

  /* world specific system */
  auto add_system(void (*system)(World&)) -> World&;
  auto add_system(const std::function<void(World&)>&& system) -> World&;

  template <typename... Args>
  auto add_system(void (*system)(World&, Args&...), Args&... args) -> World&;
  template <typename... Args>
  auto add_system(const std::function<void(World&, Args&...)>&& system,
                  Args&... args) -> World&;

  /* void system */
  auto add_system(const std::function<void()>& system) -> World&;

  void run_systems();

 private:
  using System = std::function<void()>;

  Archetypes archetypes_;
  Queries queries_;

  std::unordered_map<EntityId, EntityLocation> entities_;
  std::vector<System> systems_;

  void init();

  template <typename S, typename... Args>
  auto add_system_internal(S system) -> World&;
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

template <typename S, typename... Args>
auto World::add_system_internal(S system) -> World& {
  // setup to avoid re-calculating during game loop
  queries_.add<Args...>(archetypes_);
  systems_.emplace_back(
      [system, this]() { system(queries_.get<Args...>(archetypes_)); });
  return *this;
}

template <typename... Args>
auto World::add_system(void (*system)(Query<Args...>)) -> World& {
  return add_system_internal<void (*)(Query<Args...>), Args...>(system);
}

template <typename... Args>
auto World::add_system(std::function<void(Query<Args...>)>&& system) -> World& {
  return add_system_internal<std::function<void(Query<Args...>)>&&, Args...>(
      std::move(system));
}

inline auto World::add_system(void (*system)(World&)) -> World& {
  systems_.emplace_back([system, this]() { system(*this); });
  return *this;
}
inline auto World::add_system(const std::function<void(World&)>&& system)
    -> World& {
  systems_.emplace_back([system, this]() { system(*this); });
  return *this;
}

template <typename... Args>
auto World::add_system(void (*system)(World&, Args&...), Args&... args)
    -> World& {
  systems_.emplace_back([system, this, &args...]() { system(*this, args...); });
  return *this;
}

template <typename... Args>
auto World::add_system(const std::function<void(World&, Args&...)>&& system,
                       Args&... args) -> World& {
  systems_.emplace_back([system, this, &args...]() { system(*this, args...); });
  return *this;
}

inline auto World::add_system(const std::function<void()>& system) -> World& {
  systems_.emplace_back([system]() { system(); });
  return *this;
}

inline void World::run_systems() {
  for (auto& system : systems_) {
    system();
  }
}

inline void World::init() {
  archetypes_.event_manager().add_listener(queries_);
}

#endif
