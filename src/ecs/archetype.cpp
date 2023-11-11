
#include "ecs/archetype.h"

Archetype::Archetype(Table &&table)
    : components_{std::move(table.components())},
      table_{std::move(table)},
      archetype_id_(ArchetypeCounter::id()) {}

auto Archetype::archetype_id() const -> ArchetypeId { return archetype_id_; }
auto Archetype::table_id() const -> TableId { return table_.table_id(); }

auto Archetype::has_component(ComponentId component_id) const -> bool {
  return table_.has_component(component_id);
}

auto Archetype::components() const -> std::vector<ComponentId> {
  return components_;
}