
#include "ecs/archetype/archetype.h"

#include <algorithm>
#include <iterator>
#include <utility>

Archetype::Archetype(Table &&table)
    : table_{std::move(table)}, archetype_id_(ArchetypeCounter::id()) {}

Archetype::Archetype(Archetype &&archetype) noexcept
    : archetype_id_{std::exchange(archetype.archetype_id_,
                                  INVALID_ARCHETYPE_ID)},
      table_{std::move(archetype.table_)} {}

auto Archetype::operator=(Archetype &&archetype) noexcept -> Archetype & {
  archetype_id_ = std::exchange(archetype.archetype_id_, INVALID_ARCHETYPE_ID);
  table_ = std::move(archetype.table_);
  return *this;
}

auto Archetype::archetype_id() const -> ArchetypeId { return archetype_id_; }
auto Archetype::table_id() const -> TableId { return table_.table_id(); }
auto Archetype::is_empty() const -> bool { return table_.is_empty(); }
auto Archetype::is_valid() const -> bool {
  return archetype_id_ != INVALID_ARCHETYPE_ID && table_.is_valid();
}

auto Archetype::components() const -> ArchetypeComponents {
  return ArchetypeComponents{table_.components()};
}

auto Archetype::location(EntityId entity_id) const
    -> std::optional<EntityLocation> {
  auto it = locations_.find(entity_id);
  if (it == locations_.end()) {
    return {};
  }
  return it->second;
}
