#ifndef ECS_ARCHETYPE_ARCHETYPE_EDGES_H
#define ECS_ARCHETYPE_ARCHETYPE_EDGES_H

#include "ecs/column.h"
#include "ecs/primitive.h"

class Archetype;

class ArchetypeEdges {
  using Edge = std::unordered_map<ComponentId, Ref<Archetype>>;

 public:
  ArchetypeEdges() = default;

  void add(ComponentId component_id, Archetype &archetype);
  auto get(ComponentId component_id) -> OptionalRef<Archetype>;

  template <typename T>
  void add(Archetype &archetype) {
    add(ComponentCounter::id<T>(), archetype);
  }
  template <typename T>
  auto get() -> OptionalRef<Archetype> {
    return get(ComponentCounter::id<T>());
  }

 private:
  Edge edges_{};
};

#endif
