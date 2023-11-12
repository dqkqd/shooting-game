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

 private:
  std::set<ComponentId> components_;
};

#endif
