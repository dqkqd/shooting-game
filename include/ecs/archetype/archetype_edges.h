#ifndef ECS_ARCHETYPE_ARCHETYPE_EDGES_H
#define ECS_ARCHETYPE_ARCHETYPE_EDGES_H

#include "ecs/column.h"
#include "ecs/primitive.h"

class Archetype;

class ArchetypeEdges {
 public:
  ArchetypeEdges() = default;
  ArchetypeEdges(const ArchetypeEdges &) = delete;
  ArchetypeEdges(ArchetypeEdges &&) noexcept;
  auto operator=(const ArchetypeEdges &) -> ArchetypeEdges & = delete;
  auto operator=(ArchetypeEdges &&) noexcept -> ArchetypeEdges &;
  ~ArchetypeEdges() = default;

  template <typename T>
  void add(ArchetypeId archetype_id) {
    edges_.emplace(ComponentCounter::id<T>(), std::ref(archetype_id));
  }

  template <typename T>
  auto get() -> std::optional<ArchetypeId> {
    auto it = edges_.find(ComponentCounter::id<T>());
    if (it == edges_.end()) {
      return {};
    }
    return it->second;
  }

 private:
  std::unordered_map<ComponentId, ArchetypeId> edges_{};
};

#endif
