#ifndef ECS_ARCHETYPE_ARCHETYPES_H
#define ECS_ARCHETYPE_ARCHETYPES_H

#include <map>
#include <stdexcept>

#include "ecs/archetype/archetype.h"
#include "ecs/archetype/archetype_components.h"

class Archetypes {
 public:
  Archetypes() = default;

  template <typename T, typename... Args>
  auto add() -> OptionalRef<Archetype> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto [it, inserted] = archetypes_.emplace(
        ArchetypeComponents::create_archetype_components<T, Args...>(),
        Archetype::create_archetype<T, Args...>());
    if (inserted) {
      return it->second;
    }
    return {};
  }

  template <typename T, typename... Args>
  auto get() -> OptionalRef<Archetype> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto it = archetypes_.find(
        ArchetypeComponents::create_archetype_components<T, Args...>());
    if (it == archetypes_.end()) {
      return {};
    }
    return std::ref(it->second);
  }

  template <typename T, typename... Args>
  auto get_or_add() -> Archetype& {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto it = archetypes_.emplace(
        ArchetypeComponents::create_archetype_components<T, Args...>(),
        Archetype::create_archetype<T, Args...>());
    return it.first->second;
  }

  [[nodiscard]] auto size() const -> size_t;

 private:
  std::map<ArchetypeComponents, Archetype> archetypes_;
};

#endif
