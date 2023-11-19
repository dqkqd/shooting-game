#ifndef ECS_ARCHETYPE_EDGES_H
#define ECS_ARCHETYPE_EDGES_H

#include "ecs/column.h"
#include "ecs/primitive.h"

class ArchetypeEdges {
 public:
  ArchetypeEdges() = default;
  ~ArchetypeEdges() = default;

  ArchetypeEdges(const ArchetypeEdges&) = delete;
  auto operator=(const ArchetypeEdges&) -> ArchetypeEdges& = delete;

  ArchetypeEdges(ArchetypeEdges&& edges) noexcept
      : edges_{std::move(edges.edges_)} {}

  auto operator=(ArchetypeEdges&& edges) noexcept -> ArchetypeEdges& {
    edges_ = std::move(edges.edges_);
    return *this;
  }

  template <typename T>
  void add(ArchetypeId archetype_id) {
    edges_.emplace(ComponentCounter::id<T>(), std::ref(archetype_id));
  }

  template <typename T>
  auto get() const -> std::optional<ArchetypeId> {
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
