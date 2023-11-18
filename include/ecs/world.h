#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include <optional>

#include "ecs/archetype/archetype.h"
#include "ecs/archetype/archetypes.h"
#include "ecs/entity.h"
#include "ecs/query/base.h"

class BaseQuery;

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

  // TODO(khanhdq): return query id, and use map instead
  template <typename... Args>
  void add_query();

  template <typename... Args>
  auto run_query(size_t index) -> std::tuple<Args&...>;

  auto archetypes() -> Archetypes& { return archetypes_; }

 private:
  Archetypes archetypes_;
  std::vector<std::unique_ptr<BaseQuery>> queries_{};
  std::unordered_map<EntityId, EntityLocation> entities_;
};

#include "../../src/ecs/world.cpp"

#endif
