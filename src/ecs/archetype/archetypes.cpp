#include "ecs/archetype/archetypes.h"

#include "ecs/observer/event.h"

auto Archetypes::size() const -> size_t { return archetypes_.size(); }

auto Archetypes::get_by_id_unchecked(ArchetypeId archetype_id) -> Archetype& {
  return archetypes_[archetype_id];
}

auto Archetypes::new_archetype_id() const -> ArchetypeId {
  return static_cast<ArchetypeId>(size());
}

auto Archetypes::event_manager() -> EventManager& { return event_manager_; }

auto Archetypes::add_archetype(Archetype&& archetype) -> ArchetypeId {
  auto archetype_id = new_archetype_id();
  archetype.set_archetype_id(archetype_id);

  auto components = archetype.components().clone();
  for (const auto& component_id : components.components()) {
    finder_.add(component_id, archetype_id);
  }

  by_components_.emplace(archetype.components().clone(), archetype_id);
  archetypes_.emplace_back(std::move(archetype));

  auto event = AddArchetypeEvent(std::move(components), archetype_id);
  event_manager_.notify(&event);
  return archetype_id;
}
