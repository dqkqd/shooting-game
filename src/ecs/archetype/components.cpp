#include "ecs/archetype/components.h"

ArchetypeComponents::ArchetypeComponents(
    ArchetypeComponents &&archetype_components) noexcept
    : components_{std::exchange(archetype_components.components_, {})} {}

auto ArchetypeComponents::operator=(
    ArchetypeComponents &&archetype_components) noexcept
    -> ArchetypeComponents & {
  components_ = std::move(archetype_components.components_);
  return *this;
}

auto ArchetypeComponents::size() const -> size_t { return components_.size(); }
auto ArchetypeComponents::components() const -> const std::set<ComponentId> & {
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

auto ArchetypeComponents::clone() const -> ArchetypeComponents {
  return from_set(std::set{components_});
}

auto ArchetypeComponents::merge(ArchetypeComponents &&other) const
    -> ArchetypeComponents {
  std::set<ComponentId> intersection{components()};
  intersection.merge(other.components_);
  return from_set(std::move(intersection));
}

auto ArchetypeComponents::remove(ArchetypeComponents &&other) const
    -> ArchetypeComponents {
  std::set<ComponentId> different{components()};
  for (const auto &id : other.components()) {
    different.erase(id);
  }
  return from_set(std::move(different));
}

auto operator==(const ArchetypeComponents &lhs, const ArchetypeComponents &rhs)
    -> bool {
  return lhs.components_ == rhs.components_;
}
auto operator<(const ArchetypeComponents &lhs, const ArchetypeComponents &rhs)
    -> bool {
  return lhs.components_ < rhs.components_;
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
