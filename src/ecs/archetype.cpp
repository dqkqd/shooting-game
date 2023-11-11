
#include "ecs/archetype.h"

#include <algorithm>

Archetype::Archetype(Table &&table)
    : table_{std::move(table)},
      archetype_id_(ArchetypeCounter::id()),
      components_{table_.components()} {
  std::sort(components_.begin(), components_.end());
}

auto Archetype::archetype_id() const -> ArchetypeId { return archetype_id_; }
auto Archetype::table_id() const -> TableId { return table_.table_id(); }

auto Archetype::components() const -> std::vector<ComponentId> {
  return components_;
}