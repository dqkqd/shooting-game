
#include "ecs/archetype/archetype.h"

#include <algorithm>
#include <iterator>
#include <utility>

ArchetypeComponents::ArchetypeComponents(std::vector<ComponentId> &&components)
    : components_{std::make_move_iterator(components.begin()),
                  std::make_move_iterator(components.end())} {}

ArchetypeComponents::ArchetypeComponents(
    ArchetypeComponents &&archetype_components) noexcept
    : components_{std::exchange(archetype_components.components_, {})} {}

auto ArchetypeComponents::operator=(
    ArchetypeComponents &&archetype_components) noexcept
    -> ArchetypeComponents & {
  components_ = std::move(archetype_components.components_);
  return *this;
}

auto operator==(const ArchetypeComponents &lhs, const ArchetypeComponents &rhs)

    -> bool {
  return lhs.components_.size() == rhs.components_.size() &&
         std::all_of(lhs.components_.cbegin(), lhs.components_.cend(),
                     [&](const auto &component_id) {
                       return rhs.components_.find(component_id) !=
                              rhs.components_.end();
                     });
}

template <>
struct std::hash<ArchetypeComponents> {
  auto operator()(const ArchetypeComponents &archetype_component) const noexcept
      -> std::size_t {
    size_t seed = 0;
    for (const auto &component_id : archetype_component.components_) {
      hash_combine(seed, component_id);
    }
    return seed;
  }
};

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
