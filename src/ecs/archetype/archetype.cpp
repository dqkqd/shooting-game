
#include "ecs/archetype/archetype.h"

Archetype::Archetype(ArchetypeId archetype_id, Table &&table)
    : archetype_id_{archetype_id},
      table_{std::move(table)},
      components_{table_.components().clone()} {}

Archetype::Archetype(Archetype &&archetype) noexcept
    : archetype_id_{std::exchange(archetype.archetype_id_,
                                  INVALID_ARCHETYPE_ID)},
      table_{std::move(archetype.table_)},
      components_{std::move(archetype.components_)},
      locations_{std::move(archetype.locations_)},
      next_edges_{std::move(archetype.next_edges_)},
      prev_edges_{std::move(archetype.prev_edges_)} {}

auto Archetype::operator=(Archetype &&archetype) noexcept -> Archetype & {
  archetype_id_ = std::exchange(archetype.archetype_id_, INVALID_ARCHETYPE_ID);
  table_ = std::move(archetype.table_);
  components_ = std::move(archetype.components_);
  locations_ = std::move(archetype.locations_);
  next_edges_ = std::move(archetype.next_edges_);
  prev_edges_ = std::move(archetype.prev_edges_);
  return *this;
}

auto Archetype::archetype_id() const -> ArchetypeId { return archetype_id_; }

auto Archetype::table_id() const -> TableId { return table_.table_id(); }

auto Archetype::components() const -> const Components & { return components_; }

auto Archetype::table() -> Table & { return table_; }

auto Archetype::is_empty() const -> bool { return table_.is_empty(); }

auto Archetype::is_valid() const -> bool {
  return archetype_id_ != INVALID_ARCHETYPE_ID && table_.is_valid();
}

auto Archetype::location(EntityId entity_id) const
    -> std::optional<EntityLocation> {
  auto it = locations_.find(entity_id);
  if (it == locations_.end()) {
    return {};
  }
  return it->second;
}

void Archetype::set_archetype_id(ArchetypeId archetype_id) {
  archetype_id_ = archetype_id;
}
