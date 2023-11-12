#ifndef ECS_ARCHETYPE_ARCHETYPE_COMPONENT_H
#define ECS_ARCHETYPE_ARCHETYPE_COMPONENT_H

#include <set>

#include "ecs/column.h"
#include "ecs/primitive.h"

class ArchetypeComponents {
 public:
  friend struct std::hash<ArchetypeComponents>;

  explicit ArchetypeComponents(std::vector<ComponentId> &&components);
  ArchetypeComponents(const ArchetypeComponents &) = delete;
  ArchetypeComponents(ArchetypeComponents &&) noexcept;
  auto operator=(const ArchetypeComponents &) -> ArchetypeComponents & = delete;
  auto operator=(ArchetypeComponents &&) noexcept -> ArchetypeComponents &;
  ~ArchetypeComponents() = default;

  template <typename... Args>
  static auto create_archetype_components() -> ArchetypeComponents {
    static_assert(all_types_are_different<Args...>(),
                  "All column types must be pairwise different");

    std::vector<ComponentId> components;
    components.reserve(sizeof...(Args));
    ([&] { components.push_back(ComponentCounter::id<Args>()); }(), ...);
    return ArchetypeComponents{std::move(components)};
  }

  friend auto operator==(const ArchetypeComponents &lhs,
                         const ArchetypeComponents &rhs) -> bool;

  friend auto operator<(const ArchetypeComponents &lhs,
                        const ArchetypeComponents &rhs) -> bool;

  void add(ComponentId component_id);

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
