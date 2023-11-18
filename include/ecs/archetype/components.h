#ifndef ECS_ARCHETYPE_COMPONENT_H
#define ECS_ARCHETYPE_COMPONENT_H

#include <spdlog/spdlog.h>

#include <set>

#include "ecs/column.h"
#include "ecs/primitive.h"

class ArchetypeComponents {
 public:
  friend struct std::hash<ArchetypeComponents>;

  ArchetypeComponents() = default;
  ArchetypeComponents(const ArchetypeComponents &) = delete;
  ArchetypeComponents(ArchetypeComponents &&) noexcept;
  auto operator=(const ArchetypeComponents &) -> ArchetypeComponents & = delete;
  auto operator=(ArchetypeComponents &&) noexcept -> ArchetypeComponents &;
  ~ArchetypeComponents() = default;

  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto components() const -> const std::set<ComponentId> &;

  static auto from_vec(std::vector<ComponentId> &&components)
      -> ArchetypeComponents;
  static auto from_set(std::set<ComponentId> &&components)
      -> ArchetypeComponents;

  template <typename... Args>
  static auto from_types() -> ArchetypeComponents {
    static_assert(all_types_are_different<Args...>(),
                  "All column types must be pairwise different");
    std::set<ComponentId> components;
    (components.insert(ComponentCounter::id<Args>()), ...);
    return from_set(std::move(components));
  }

  [[nodiscard]] auto clone() const -> ArchetypeComponents;

  template <typename... Args>
  [[nodiscard]] auto clone_with() const -> std::optional<ArchetypeComponents> {
    auto archetype = clone().merge(ArchetypeComponents::from_types<Args...>());
    if (archetype.size() != size() + sizeof...(Args)) {
      spdlog::warn(
          "Trying to use `clone_with` with duplicated or existed types");
      return {};
    }
    return std::move(archetype);
  }

  template <typename... Args>
  [[nodiscard]] auto clone_without() const
      -> std::optional<ArchetypeComponents> {
    auto archetype = clone().remove(ArchetypeComponents::from_types<Args...>());
    if (archetype.size() != size() - sizeof...(Args)) {
      spdlog::warn(
          "Trying to use `clone_without` with duplicated or non existed types");
      return {};
    }
    return std::move(archetype);
  }

  friend auto operator==(const ArchetypeComponents &lhs,
                         const ArchetypeComponents &rhs) -> bool;

  friend auto operator<(const ArchetypeComponents &lhs,
                        const ArchetypeComponents &rhs) -> bool;

 private:
  std::set<ComponentId> components_;

  auto merge(ArchetypeComponents &&other) const -> ArchetypeComponents;
  auto remove(ArchetypeComponents &&other) const -> ArchetypeComponents;
};

#endif
