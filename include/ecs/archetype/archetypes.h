#ifndef ECS_ARCHETYPE_ARCHETYPES_H
#define ECS_ARCHETYPE_ARCHETYPES_H

#include <map>
#include <stdexcept>

#include "ecs/archetype/archetype.h"

class ArchetypesFinder {
 public:
  ArchetypesFinder() = default;

  void add(ComponentId component_id, ArchetypeId archetype_id);
  template <typename... Args>
  auto get() const -> std::vector<ArchetypeId> {
    std::unordered_map<ArchetypeId, int> archetypes_occurrences;
    (
        [&] {
          for (const auto &archetype_id :
               by_component_.at(ComponentCounter::id<Args>())) {
            ++archetypes_occurrences[archetype_id];
          }
        }(),
        ...);
    std::vector<ArchetypeId> results;
    for (const auto &[archetype_id, count] : archetypes_occurrences) {
      if (count == sizeof...(Args)) {
        results.emplace_back(archetype_id);
      }
    }
    std::reverse(results.begin(), results.end());
    return results;
  }

 private:
  std::unordered_map<ComponentId, std::vector<ArchetypeId>> by_component_;
};

class Archetypes {
 public:
  Archetypes() = default;

  auto get_by_id_unchecked(ArchetypeId archetype_id) -> Archetype &;
  auto finder() -> ArchetypesFinder & { return finder_; }

  template <typename T, typename... Args>
  auto add() -> std::optional<ArchetypeId> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto archetype = Archetype::create_archetype<T, Args...>();
    if (by_components_.count(archetype.components())) {
      return {};
    }
    return add_archetype(std::move(archetype));
  }

  template <typename T, typename... Args>
  auto get() -> std::optional<ArchetypeId> {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto it =
        by_components_.find(ArchetypeComponents::from_types<T, Args...>());
    if (it == by_components_.end()) {
      return {};
    }
    return it->second;
  }

  template <typename T, typename... Args>
  auto get_or_add() -> ArchetypeId {
    static_assert(all_types_are_different<T, Args...>(),
                  "All column types must be pairwise different");
    auto archetype = Archetype::create_archetype<T, Args...>();
    auto it = by_components_.find(archetype.components());
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetype_id;
    }
    return add_archetype(std::move(archetype));
  }

  template <typename T>
  auto get_or_add_next_archetype(ArchetypeId archetype_id) -> ArchetypeId {
    /* Find the next archetype on the graph, add one if there is none */

    auto archetype = archetypes_[archetype_id].clone_with<T>();
    auto it = by_components_.find(archetype.components());
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetype_id;
    }

    auto next_archetype_id = add_archetype(std::move(archetype));
    add_edge<T>(archetype_id, next_archetype_id);
    return next_archetype_id;
  }

  template <typename T>
  auto get_or_add_prev_archetype(ArchetypeId archetype_id) -> ArchetypeId {
    /* Find the prev archetype on the graph, add one if there is none */
    auto archetype = archetypes_[archetype_id].clone_without<T>();
    auto it = by_components_.find(archetype.components());
    if (it != by_components_.end()) {
      auto archetype_id = it->second;
      return archetype_id;
    }

    auto prev_archetype_id = add_archetype(std::move(archetype));
    add_edge<T>(prev_archetype_id, archetype_id);
    return prev_archetype_id;
  }

  [[nodiscard]] auto size() const -> size_t;

 private:
  [[nodiscard]] auto new_archetype_id() const -> ArchetypeId;

  template <typename T>
  void add_edge(ArchetypeId archetype_id, ArchetypeId next_archetype_id) {
    archetypes_[archetype_id].add_next_edge<T>(next_archetype_id);
    archetypes_[next_archetype_id].add_prev_edge<T>(archetype_id);
  }
  auto add_archetype(Archetype &&archetype) -> ArchetypeId;

  std::vector<Archetype> archetypes_;
  std::map<ArchetypeComponents, ArchetypeId> by_components_;
  ArchetypesFinder finder_;
};

#endif
