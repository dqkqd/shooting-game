
#include "ecs/archetype/archetype_edges.h"

void ArchetypeEdges::add(ComponentId component_id, Archetype &archetype) {
  edges_.emplace(component_id, std::ref(archetype));
}

auto ArchetypeEdges::get(ComponentId component_id) -> OptionalRef<Archetype> {
  auto it = edges_.find(component_id);
  if (it == edges_.end()) {
    return {};
  }
  return it->second;
}
