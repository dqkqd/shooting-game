#ifndef ECS_ARCHETYPE_ARCHETYPE_COMPONENT_H
#define ECS_ARCHETYPE_ARCHETYPE_COMPONENT_H

#include <spdlog/spdlog.h>

#include <set>
#include <stdexcept>

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

  static auto from_vec(std::vector<ComponentId> &&components)
      -> ArchetypeComponents;
  static auto from_set(std::set<ComponentId> &&components)
      -> ArchetypeComponents;

  template <typename T, typename... Args>
  static auto from_types() -> ArchetypeComponents {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    return ArchetypeComponents(ComponentCounter::id<T>(),
                               ComponentCounter::id<Args>()...);
  }

  [[nodiscard]] auto clone() const -> ArchetypeComponents;

  template <typename T, typename... Args>
  [[nodiscard]] auto clone_with() const -> std::optional<ArchetypeComponents> {
    return clone_with(ComponentCounter::id<T>(),
                      ComponentCounter::id<Args>()...);
  }

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  [[nodiscard]] auto clone_with(ComponentId component_id,
                                Args... component_ids) const
      -> std::optional<ArchetypeComponents> {
    std::set components{components_};
    components.insert(component_id);
    (components.insert(component_ids), ...);
    if (components.size() != components_.size() + 1 + sizeof...(Args)) {
      spdlog::warn(
          "Trying to use `clone_with` with duplicated or existed types");
      return {};
    }
    return ArchetypeComponents::from_set(std::move(components));
  }

  template <typename T, typename... Args>
  [[nodiscard]] auto clone_without() const
      -> std::optional<ArchetypeComponents> {
    return clone_without(ComponentCounter::id<T>(),
                         ComponentCounter::id<Args>()...);
  }

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  [[nodiscard]] auto clone_without(ComponentId component_id,
                                   Args... component_ids) const
      -> std::optional<ArchetypeComponents> {
    std::set components{components_};
    components.erase(component_id);
    (components.erase(component_ids), ...);
    if (components.size() != components_.size() - 1 - sizeof...(Args)) {
      spdlog::warn(
          "Trying to use `clone_without` with duplicated or non existed types");
      return {};
    }
    return ArchetypeComponents::from_set(std::move(components));
  }

  friend auto operator==(const ArchetypeComponents &lhs,
                         const ArchetypeComponents &rhs) -> bool;

  friend auto operator<(const ArchetypeComponents &lhs,
                        const ArchetypeComponents &rhs) -> bool;

  [[nodiscard]] auto as_raw() const -> const std::set<ComponentId> &;

 private:
  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  explicit ArchetypeComponents(ComponentId component_id,
                               Args... component_ids) {
    components_.insert(component_id);
    (components_.insert(component_ids), ...);
  }

  [[nodiscard]] auto has_component(ComponentId component_id) const -> bool;

  std::set<ComponentId> components_;
};

#endif
