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
    auto components =
        ArchetypeComponents::create_archetype_components<T, Args...>();
    if (by_components_.find(components) != by_components_.end()) {
      return {};
    }
    auto archetype = Archetype::create_archetype<T, Args...>();
    by_components_.emplace(std::move(components), archetype.archetype_id());
    auto [it, _] =
        archetypes_.emplace(archetype.archetype_id(), std::move(archetype));
    return std::ref(it->second);
  }

  template <typename T, typename... Args>
  auto get() -> OptionalRef<Archetype> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto it = by_components_.find(
        ArchetypeComponents::create_archetype_components<T, Args...>());
    if (it == by_components_.end()) {
      return {};
    }
    auto archetype_id = it->second;
    // we can always sure that archetype existed
    return std::ref(archetypes_.find(archetype_id)->second);
  }

  template <typename T, typename... Args>
  auto get_or_add() -> Archetype& {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");

    auto components =
        ArchetypeComponents::create_archetype_components<T, Args...>();
    auto archetype = Archetype::create_archetype<T, Args...>();

    by_components_.emplace(std::move(components), archetype.archetype_id());
    auto [it, _] =
        archetypes_.emplace(archetype.archetype_id(), std::move(archetype));
    return it->second;
  }

  template <typename T>
  auto get_or_add_next_archetype(Archetype& archetype) -> Archetype& {
    /* Find the next archetype on the graph, add one if there is none */

    if (archetype.has_components<T>()) {
      throw std::runtime_error(fmt::format(
          "Component type `{}` has already existed in archetype `{}`",
          typeid(T).name(), archetype.archetype_id()));
    }

    auto components =
        ArchetypeComponents(archetype.components().clone_with<T>());
    auto it = by_components_.find(components);
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetypes_.find(archetype_id)->second;
    }

    Archetype next_archetype = archetype.clone_with<T>();
    by_components_.emplace(std::move(components),
                           next_archetype.archetype_id());
    auto [archetype_it, _] = archetypes_.emplace(next_archetype.archetype_id(),
                                                 std::move(next_archetype));
    archetype.add_next_edge<T>(archetype_it->second);
    return archetype_it->second;
  }

  template <typename T>
  auto get_or_add_prev_archetype(Archetype& archetype) -> Archetype& {
    /* Find the prev archetype on the graph, add one if there is none */

    if (!archetype.has_components<T>()) {
      throw std::runtime_error(
          fmt::format("Component type `{}` does not exist in archetype `{}`",
                      typeid(T).name(), archetype.archetype_id()));
    }

    auto components =
        ArchetypeComponents(archetype.components().clone_without<T>());
    auto it = by_components_.find(components);
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetypes_.find(archetype_id)->second;
    }

    Archetype prev_archetype = archetype.clone_without<T>();
    by_components_.emplace(std::move(components),
                           prev_archetype.archetype_id());
    auto [archetype_it, _] = archetypes_.emplace(prev_archetype.archetype_id(),
                                                 std::move(prev_archetype));
    archetype.add_prev_edge<T>(archetype_it->second);
    return archetype_it->second;
  }

  [[nodiscard]] auto size() const -> size_t;

 private:
  // TODO(khanhdq) use vector later without ArchetypeId
  std::unordered_map<ArchetypeId, Archetype> archetypes_;
  std::map<ArchetypeComponents, ArchetypeId> by_components_;
};

#endif
