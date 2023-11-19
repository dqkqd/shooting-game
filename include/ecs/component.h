#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <spdlog/spdlog.h>

#include <set>

#include "ecs/column.h"
#include "ecs/primitive.h"

class Components {
 public:
  friend struct std::hash<Components>;

  Components() = default;
  Components(const Components &) = delete;
  Components(Components &&) noexcept;
  auto operator=(const Components &) -> Components & = delete;
  auto operator=(Components &&) noexcept -> Components &;
  ~Components() = default;

  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto components() const -> const std::set<ComponentId> &;

  [[nodiscard]] auto has_component(ComponentId component_id) const -> bool {
    return components_.count(component_id) > 0;
  }
  template <typename... Args>
  [[nodiscard]] auto has_components() const -> bool {
    static_assert(all_types_are_different<Args...>());
    return sizeof...(Args) > 0 &&
           (... && has_component(ComponentCounter::id<Args>()));
  }

  static auto from_vec(std::vector<ComponentId> &&components) -> Components;
  static auto from_set(std::set<ComponentId> &&components) -> Components;

  template <typename... Args>
  static auto from_types() -> Components {
    static_assert(all_types_are_different<Args...>(),
                  "All column types must be pairwise different");
    std::set<ComponentId> components;
    (components.insert(ComponentCounter::id<Args>()), ...);
    return from_set(std::move(components));
  }

  [[nodiscard]] auto clone() const -> Components;

  template <typename... Args>
  [[nodiscard]] auto clone_with() const -> std::optional<Components> {
    auto archetype = clone().merge(Components::from_types<Args...>());
    if (archetype.size() != size() + sizeof...(Args)) {
      spdlog::warn(
          "Trying to use `clone_with` with duplicated or existed types");
      return {};
    }
    return std::move(archetype);
  }

  template <typename... Args>
  [[nodiscard]] auto clone_without() const -> std::optional<Components> {
    auto archetype = clone().remove(Components::from_types<Args...>());
    if (archetype.size() != size() - sizeof...(Args)) {
      spdlog::warn(
          "Trying to use `clone_without` with duplicated or non existed types");
      return {};
    }
    return std::move(archetype);
  }

  friend auto operator==(const Components &lhs, const Components &rhs) -> bool;

  friend auto operator<(const Components &lhs, const Components &rhs) -> bool;

 private:
  std::set<ComponentId> components_;

  auto merge(Components &&other) const -> Components;
  auto remove(Components &&other) const -> Components;
};

#endif
