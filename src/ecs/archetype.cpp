
#include "ecs/archetype.h"

#include <algorithm>
#include <utility>

Archetype::Archetype(Table &&table)
    : table_{std::move(table)},
      archetype_id_(ArchetypeCounter::id()),
      components_{table_.components()} {
  std::sort(components_.begin(), components_.end());
}

Archetype::Archetype(Archetype &&archetype) noexcept
    : archetype_id_{std::exchange(archetype.archetype_id_,
                                  INVALID_ARCHETYPE_ID)},
      table_{std::move(archetype.table_)},
      components_{std::exchange(archetype.components_, {})} {}

auto Archetype::operator=(Archetype &&archetype) noexcept -> Archetype & {
  archetype_id_ = std::exchange(archetype.archetype_id_, INVALID_ARCHETYPE_ID);
  table_ = std::move(archetype.table_);
  components_ = std::exchange(archetype.components_, {});
  return *this;
}

auto Archetype::archetype_id() const -> ArchetypeId { return archetype_id_; }
auto Archetype::table_id() const -> TableId { return table_.table_id(); }
auto Archetype::is_empty() const -> bool { return table_.is_empty(); }
auto Archetype::is_valid() const -> bool {
  return archetype_id_ != INVALID_ARCHETYPE_ID && table_.is_valid();
}

auto Archetype::components() const -> std::vector<ComponentId> {
  return components_;
}

auto Archetype::location(EntityId entity_id) const
    -> std::optional<EntityLocation> {
  auto it = locations_.find(entity_id);
  if (it == locations_.end()) {
    return {};
  }
  return it->second;
}

auto Archetype::move_entity_to_other(EntityId entity_id, Archetype &other)
    -> std::optional<EntityLocation> {
  auto entity_location = location(entity_id);
  if (!entity_location.has_value()) {
    return {};
  }

  auto new_height =
      table_.move_row_to_other(entity_location->row, other.table_);
  locations_.erase(entity_id);

  if (!new_height.has_value() || new_height == 0) {
    return {};
  }
  auto location = EntityLocation{.archetype_id = other.archetype_id(),
                                 .table_id = other.table_id(),
                                 .row = new_height.value() - 1};
  other.locations_[entity_id] = location;
  return location;
}