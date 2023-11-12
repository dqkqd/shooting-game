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

  template <typename T>
  auto get_or_add_next_archetype(Archetype& archetype) -> Archetype& {
    /* Find the next archetype on the graph, add one if there is none */

    if (archetype.has_components<T>()) {
      throw std::runtime_error(fmt::format(
          "Component type `{}` has already existed in archetype `{}`",
          typeid(T).name(), archetype.archetype_id()));
    }

    auto [it, inserted] = archetypes_.emplace(
        ArchetypeComponents(archetype.components().clone_with<T>()),
        archetype.clone_with<T>());
    archetype.add_next_edge<T>(it->second);
    return it->second;
  }

  template <typename T>
  auto get_or_add_prev_archetype(Archetype& archetype) -> Archetype& {
    /* Find the prev archetype on the graph, add one if there is none */

    if (!archetype.has_components<T>()) {
      throw std::runtime_error(
          fmt::format("Component type `{}` does not exist in archetype `{}`",
                      typeid(T).name(), archetype.archetype_id()));
    }

    auto [it, inserted] = archetypes_.emplace(
        ArchetypeComponents(archetype.components().clone_without<T>()),
        archetype.clone_without<T>());
    archetype.add_prev_edge<T>(it->second);
    return it->second;
  }

  [[nodiscard]] auto size() const -> size_t;

 private:
  std::map<ArchetypeComponents, Archetype> archetypes_;
};

#endif
