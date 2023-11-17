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

  template <typename T, typename... Args>
  auto clone_with() -> std::optional<ArchetypeComponents> {
    return clone_with(ComponentCounter::id<T>(),
                      ComponentCounter::id<Args>()...);
  }

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  auto clone_with(ComponentId component_id, Args... component_ids)
      -> std::optional<ArchetypeComponents> {
    auto one_component_existed =
        (has_component(component_id) || ... || has_component(component_ids));
    if (one_component_existed) {
      spdlog::error(
          "All components must not be existed in order to `clone_with`");
      return {};
    }

    std::vector<ComponentId> components{components_.begin(), components_.end()};
    components.emplace_back(component_id);
    (components.emplace_back(component_ids), ...);

    auto expected_size = components_.size() + 1 + sizeof...(Args);
    if (components.size() != expected_size) {
      spdlog::error("Can not `clone_with` with duplicated ids");
      return {};
    }

    return ArchetypeComponents::from_vec(std::move(components));
  }

  template <typename T, typename... Args>
  auto clone_without() -> std::optional<ArchetypeComponents> {
    return clone_without(ComponentCounter::id<T>(),
                         ComponentCounter::id<Args>()...);
  }

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  auto clone_without(ComponentId component_id, Args... component_ids)
      -> std::optional<ArchetypeComponents> {
    auto all_components_existed =
        (has_component(component_id) && ... && has_component(component_ids));
    if (!all_components_existed) {
      spdlog::error(
          "All components must be existed in order to `clone_without`");
      return {};
    }

    std::set<ComponentId> components{components_.begin(), components_.end()};
    components.erase(component_id);
    (components.erase(component_ids), ...);

    auto expected_size = components_.size() - (1 + sizeof...(Args));
    if (components.size() != expected_size) {
      spdlog::error("Can not `clone_without` with duplicated ids");
      return {};
    }

    std::vector<ComponentId> vector_components{
        std::make_move_iterator(components.begin()),
        std::make_move_iterator(components.end())};

    return ArchetypeComponents::from_vec(std::move(vector_components));
  }

  friend auto operator==(const ArchetypeComponents &lhs,
                         const ArchetypeComponents &rhs) -> bool;

  friend auto operator<(const ArchetypeComponents &lhs,
                        const ArchetypeComponents &rhs) -> bool;

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
