#ifndef ECS_ARCHETYPE_ARCHETYPES_H
#define ECS_ARCHETYPE_ARCHETYPES_H

#include <map>
#include <stdexcept>

#include "ecs/archetype/archetype.h"
#include "ecs/archetype/archetype_components.h"

class Archetypes {
 public:
  Archetypes() = default;

  auto get_by_id_unchecked(ArchetypeId archetype_id) -> Archetype &;

  template <typename T, typename... Args>
  auto add() -> std::optional<ArchetypeId> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto components =
        ArchetypeComponents::create_archetype_components<T, Args...>();
    if (by_components_.find(components) != by_components_.end()) {
      return {};
    }

    auto archetype_id = size();
    add_component_and_archetype(
        std::move(components),
        Archetype::create_archetype<T, Args...>(archetype_id));

    return archetype_id;
  }

  template <typename T, typename... Args>
  auto get() -> std::optional<ArchetypeId> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto it = by_components_.find(
        ArchetypeComponents::create_archetype_components<T, Args...>());
    if (it == by_components_.end()) {
      return {};
    }
    return it->second;
  }

  template <typename T, typename... Args>
  auto get_or_add() -> ArchetypeId {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");

    auto components =
        ArchetypeComponents::create_archetype_components<T, Args...>();
    auto it = by_components_.find(components);

    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetype_id;
    }

    auto archetype_id = static_cast<ArchetypeId>(size());
    add_component_and_archetype(
        std::move(components),
        Archetype::create_archetype<T, Args...>(archetype_id));

    return archetype_id;
  }

  template <typename T>
  auto get_or_add_next_archetype(ArchetypeId archetype_id)
      -> std::optional<ArchetypeId> {
    /* Find the next archetype on the graph, add one if there is none */

    auto components = archetypes_[archetype_id].components().clone_with<T>();
    if (!components.has_value()) {
      spdlog::error("Component type `{}` has already existed in archetype `{}`",
                    typeid(T).name(), archetype_id);
      return {};
    }

    auto it = by_components_.find(*components);
    if (it != by_components_.end()) {
      return it->second;
    }

    auto next_archetype_id = static_cast<ArchetypeId>(size());
    add_component_and_archetype(
        std::move(components.value()),
        archetypes_[archetype_id].clone_with<T>(next_archetype_id));
    add_edge<T>(archetype_id, next_archetype_id);

    return next_archetype_id;
  }

  template <typename T>
  auto get_or_add_prev_archetype(ArchetypeId archetype_id)
      -> std::optional<ArchetypeId> {
    /* Find the prev archetype on the graph, add one if there is none */

    auto components = archetypes_[archetype_id].components().clone_without<T>();
    if (!components.has_value()) {
      spdlog::error("Component type `{}` does not exist in archetype `{}`",
                    typeid(T).name(), archetype_id);
      return {};
    }

    auto it = by_components_.find(*components);
    if (it != by_components_.end()) {
      return it->second;
    }

    auto prev_archetype_id = static_cast<ArchetypeId>(size());
    add_component_and_archetype(
        std::move(components.value()),
        archetypes_[archetype_id].clone_without<T>(prev_archetype_id));
    add_edge<T>(prev_archetype_id, archetype_id);

    return prev_archetype_id;
  }

  [[nodiscard]] auto size() const -> size_t;

 private:
  template <typename T>
  void add_edge(ArchetypeId archetype_id, ArchetypeId next_archetype_id) {
    archetypes_[archetype_id].add_next_edge<T>(next_archetype_id);
    archetypes_[next_archetype_id].add_prev_edge<T>(archetype_id);
  }
  void add_component_and_archetype(ArchetypeComponents &&component,
                                   Archetype &&archetype);

  std::vector<Archetype> archetypes_;
  std::map<ArchetypeComponents, ArchetypeId> by_components_;
};

#endif
