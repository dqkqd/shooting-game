#ifndef ECS_ARCHETYPE_ARCHETYPE_COMPONENT_H
#define ECS_ARCHETYPE_ARCHETYPE_COMPONENT_H

#include <set>
#include <stdexcept>

#include "ecs/column.h"
#include "ecs/primitive.h"

class ArchetypeComponents {
 public:
  friend struct std::hash<ArchetypeComponents>;

  explicit ArchetypeComponents(std::vector<ComponentId> &&components);

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  explicit ArchetypeComponents(ComponentId component_id,
                               Args... component_ids) {
    components_.insert(component_id);
    (components_.insert(component_ids), ...);
  }

  ArchetypeComponents(const ArchetypeComponents &) = delete;
  ArchetypeComponents(ArchetypeComponents &&) noexcept;
  auto operator=(const ArchetypeComponents &) -> ArchetypeComponents & = delete;
  auto operator=(ArchetypeComponents &&) noexcept -> ArchetypeComponents &;
  ~ArchetypeComponents() = default;

  template <typename T, typename... Args>
  static auto create_archetype_components() -> ArchetypeComponents {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    return ArchetypeComponents(ComponentCounter::id<T>(),
                               ComponentCounter::id<Args>()...);
  }

  template <typename T, typename... Args>
  auto clone_with() -> ArchetypeComponents {
    return clone_with(ComponentCounter::id<T>(),
                      ComponentCounter::id<Args>()...);
  }

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  auto clone_with(ComponentId component_id, Args... component_ids)
      -> ArchetypeComponents {
    auto component_existed = has_components(component_id, component_ids...);
    if (component_existed) {
      throw std::runtime_error("All components must be unique");
    }
    std::vector<ComponentId> components{components_.begin(), components_.end()};
    components.emplace_back(component_id);
    (components.emplace_back(component_ids), ...);

    auto expected_size = components_.size() + 1 + sizeof...(Args);
    if (components.size() < expected_size) {
      throw std::runtime_error("All components must be unique");
    }

    return ArchetypeComponents(std::move(components));
  }

  friend auto operator==(const ArchetypeComponents &lhs,
                         const ArchetypeComponents &rhs) -> bool;

  friend auto operator<(const ArchetypeComponents &lhs,
                        const ArchetypeComponents &rhs) -> bool;

  [[nodiscard]] auto has_component_id(ComponentId component_id) const -> bool;
  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  [[nodiscard]] auto has_components(ComponentId component_id,
                                    Args... component_ids) -> bool {
    return has_component_id(component_id) &&
           (... && has_component_id(component_ids));
  }
  template <typename T, typename... Args>
  [[nodiscard]] auto has_components() -> bool {
    return has_components(ComponentCounter::id<T>(),
                          ComponentCounter::id<Args>()...);
  }

 private:
  std::set<ComponentId> components_;
};

#endif
