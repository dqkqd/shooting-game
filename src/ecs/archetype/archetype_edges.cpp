#include "ecs/archetype/archetype_edges.h"

ArchetypeEdges::ArchetypeEdges(ArchetypeEdges&& edges) noexcept
    : edges_{std::move(edges.edges_)} {}

auto ArchetypeEdges::operator=(ArchetypeEdges&& edges) noexcept
    -> ArchetypeEdges& {
  edges_ = std::move(edges.edges_);
  return *this;
}
