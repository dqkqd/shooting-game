#ifndef ECS_ARCHETYPE_ARCHETYPE_EDGES_H
#define ECS_ARCHETYPE_ARCHETYPE_EDGES_H

#include "ecs/column.h"
#include "ecs/primitive.h"

class Archetype;

class ArchetypeEdges {
 public:
  ArchetypeEdges() = default;

  template <typename T>
  void add(Archetype &archetype) {
    edges_.emplace(ComponentCounter::id<T>(), std::ref(archetype));
  }

  template <typename T>
  auto get() -> OptionalRef<Archetype> {
    auto it = edges_.find(ComponentCounter::id<T>());
    if (it == edges_.end()) {
      return {};
    }
    return it->second;
  }

 private:
  std::unordered_map<ComponentId, Ref<Archetype>> edges_{};
};

#endif
