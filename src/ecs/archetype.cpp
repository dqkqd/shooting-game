
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

auto Archetype::components() const -> std::vector<ComponentId> {
  return components_;
}