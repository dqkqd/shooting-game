#include "ecs/component.h"

Components::Components(Components &&archetype_components) noexcept
    : components_{std::exchange(archetype_components.components_, {})} {}

auto Components::operator=(Components &&archetype_components) noexcept
    -> Components & {
  components_ = std::move(archetype_components.components_);
  return *this;
}

auto Components::size() const -> size_t { return components_.size(); }
auto Components::components() const -> const std::set<ComponentId> & {
  return components_;
}

auto Components::from_vec(std::vector<ComponentId> &&components) -> Components {
  return from_set({std::make_move_iterator(components.begin()),
                   std::make_move_iterator(components.end())});
}

auto Components::from_set(std::set<ComponentId> &&components) -> Components {
  Components archetype_components;
  archetype_components.components_ = std::move(components);
  return archetype_components;
}

auto Components::clone() const -> Components {
  return from_set(std::set{components_});
}

auto Components::merge(Components &&other) const -> Components {
  std::set<ComponentId> intersection{components()};
  intersection.merge(other.components_);
  return from_set(std::move(intersection));
}

auto Components::remove(Components &&other) const -> Components {
  std::set<ComponentId> different{components()};
  for (const auto &id : other.components()) {
    different.erase(id);
  }
  return from_set(std::move(different));
}

auto operator==(const Components &lhs, const Components &rhs) -> bool {
  return lhs.components_ == rhs.components_;
}
auto operator<(const Components &lhs, const Components &rhs) -> bool {
  return lhs.components_ < rhs.components_;
}

template <>
struct std::hash<Components> {
  auto operator()(const Components &archetype_component) const noexcept
      -> std::size_t {
    size_t seed = 0;
    for (const auto &component_id : archetype_component.components_) {
      hash_combine(seed, component_id);
    }
    return seed;
  }
};