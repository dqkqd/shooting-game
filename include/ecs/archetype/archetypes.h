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

    auto archetype_id = size();
    by_components_.emplace(std::move(components), archetype_id);
    archetypes_.emplace_back(
        Archetype::create_archetype<T, Args...>(archetype_id));
    return std::ref(archetypes_.back());
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
    return std::ref(archetypes_[archetype_id]);
  }

  template <typename T, typename... Args>
  auto get_or_add() -> Archetype& {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");

    auto components =
        ArchetypeComponents::create_archetype_components<T, Args...>();
    auto it = by_components_.find(components);

    if (by_components_.find(components) != by_components_.end()) {
      auto archetype_id = it->second;
      return archetypes_[archetype_id];
    }

    auto archetype_id = size();
    by_components_.emplace(std::move(components), archetype_id);
    archetypes_.emplace_back(
        Archetype::create_archetype<T, Args...>(archetype_id));
    return archetypes_.back();
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
        ArchetypeComponents(archetype.components()
                                .clone_with<T>()
                                .value());  // TODO(khanhdq): remove this value
    auto it = by_components_.find(components);
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetypes_[archetype_id];
    }

    auto next_archetype_id = size();
    by_components_.emplace(std::move(components), next_archetype_id);
    archetypes_.emplace_back(archetype.clone_with<T>(next_archetype_id));
    archetype.add_next_edge<T>(archetypes_.back());
    return archetypes_.back();
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
        ArchetypeComponents(archetype.components()
                                .clone_without<T>()
                                .value());  // TODO(khanhdq): remove this value
    auto it = by_components_.find(components);
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetypes_[archetype_id];
    }

    auto prev_archetype_id = size();
    by_components_.emplace(std::move(components), prev_archetype_id);
    archetypes_.emplace_back(archetype.clone_without<T>(prev_archetype_id));
    archetype.add_prev_edge<T>(archetypes_.back());
    return archetypes_.back();
  }

  [[nodiscard]] auto size() const -> size_t;

 private:
  std::vector<Archetype> archetypes_;
  std::map<ArchetypeComponents, ArchetypeId> by_components_;
};

#endif
