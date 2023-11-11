#include "ecs/archetype/archetype_components.h"

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