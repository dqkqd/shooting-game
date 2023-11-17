#include "ecs/archetype/archetype_components.h"

#include <utility>

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
  return lhs.components_ == rhs.components_;
}
auto operator<(const ArchetypeComponents &lhs, const ArchetypeComponents &rhs)
    -> bool {
  return lhs.components_ < rhs.components_;
}

auto ArchetypeComponents::clone() const -> ArchetypeComponents {
  return ArchetypeComponents::from_set(std::set{components_});
}

auto ArchetypeComponents::size() const -> size_t { return components_.size(); }

auto ArchetypeComponents::has_component(ComponentId component_id) const
    -> bool {
  return components_.count(component_id) > 0;
}

auto ArchetypeComponents::as_raw() const -> const std::set<ComponentId> & {
  return components_;
}

auto ArchetypeComponents::from_vec(std::vector<ComponentId> &&components)
    -> ArchetypeComponents {
  return from_set({std::make_move_iterator(components.begin()),
                   std::make_move_iterator(components.end())});
}

auto ArchetypeComponents::from_set(std::set<ComponentId> &&components)
    -> ArchetypeComponents {
  ArchetypeComponents archetype_components;
  archetype_components.components_ = std::move(components);
  return archetype_components;
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
