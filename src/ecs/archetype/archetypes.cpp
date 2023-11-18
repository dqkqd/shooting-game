#include "ecs/archetype/archetypes.h"

void ArchetypesFinder::add(ComponentId component_id, ArchetypeId archetype_id) {
  by_component_[component_id].emplace_back(archetype_id);
}

auto Archetypes::size() const -> size_t { return archetypes_.size(); }

auto Archetypes::get_by_id_unchecked(ArchetypeId archetype_id) -> Archetype& {
  return archetypes_[archetype_id];
}

auto Archetypes::new_archetype_id() const -> ArchetypeId {
  return static_cast<ArchetypeId>(size());
}

auto Archetypes::add_archetype(Archetype&& archetype) -> ArchetypeId {
  auto archetype_id = new_archetype_id();
  archetype.set_archetype_id(archetype_id);

  auto components = archetype.components().clone();
  for (const auto& component_id : components.components()) {
    finder_.add(component_id, archetype_id);
  }

  by_components_.emplace(archetype.components().clone(), archetype_id);
  archetypes_.emplace_back(std::forward<Archetype>(archetype));

  return archetype_id;
}
