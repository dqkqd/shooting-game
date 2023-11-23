#ifndef ECS_ARCHETYPE_ARCHETYPES_H
#define ECS_ARCHETYPE_ARCHETYPES_H

#include <map>
#include <stdexcept>

#include "ecs/archetype/archetype.h"
#include "ecs/archetype/finder.h"
#include "ecs/observer/observer.h"

class Archetypes {
 public:
  Archetypes() = default;
  Archetypes(const Archetypes &) = delete;
  Archetypes(Archetypes &&) = delete;
  auto operator=(const Archetypes &) -> Archetypes & = delete;
  auto operator=(Archetypes &&) -> Archetypes & = delete;
  ~Archetypes() = default;

  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto get_by_id_unchecked(ArchetypeId archetype_id)
      -> Archetype &;

  template <typename... Args>
  [[nodiscard]] auto find() const -> std::vector<ArchetypeId>;

  template <typename T, typename... Args>
  [[nodiscard]] auto add() -> std::optional<ArchetypeId>;

  template <typename T, typename... Args>
  [[nodiscard]] auto get() -> std::optional<ArchetypeId>;

  template <typename T, typename... Args>
  [[nodiscard]] auto get_or_add() -> ArchetypeId;

  template <typename T>
  [[nodiscard]] auto get_or_add_next_archetype(ArchetypeId archetype_id)
      -> ArchetypeId;

  template <typename T>
  [[nodiscard]] auto get_or_add_prev_archetype(ArchetypeId archetype_id)
      -> ArchetypeId;

  [[nodiscard]] auto event_manager() -> EventManager &;

 private:
  std::vector<Archetype> archetypes_;
  std::map<Components, ArchetypeId> by_components_;
  ArchetypesFinder finder_;

  EventManager event_manager_;

  [[nodiscard]] auto new_archetype_id() const -> ArchetypeId;

  template <typename T>
  void add_edge(ArchetypeId archetype_id, ArchetypeId next_archetype_id);

  auto add_archetype(Archetype &&archetype) -> ArchetypeId;
};

/* put these definition here since they are template methods */

template <typename... Args>
auto Archetypes::find() const -> std::vector<ArchetypeId> {
  return finder_.find<Args...>();
}

template <typename T, typename... Args>
auto Archetypes::add() -> std::optional<ArchetypeId> {
  static_assert(all_types_are_different<T, Args...>());

  auto archetype = Archetype::create_archetype<T, Args...>();
  if (by_components_.count(archetype.components())) {
    return {};
  }
  return add_archetype(std::move(archetype));
}

template <typename T, typename... Args>
auto Archetypes::get() -> std::optional<ArchetypeId> {
  static_assert(all_types_are_different<T, Args...>());

  auto it = by_components_.find(Components::from_types<T, Args...>());
  if (it == by_components_.end()) {
    return {};
  }
  return it->second;
}

template <typename T, typename... Args>
auto Archetypes::get_or_add() -> ArchetypeId {
  static_assert(all_types_are_different<T, Args...>());

  auto archetype = Archetype::create_archetype<T, Args...>();
  auto it = by_components_.find(archetype.components());
  if (it != by_components_.end()) {
    auto archetype_id = it->second;
    return archetype_id;
  }
  return add_archetype(std::move(archetype));
}

template <typename T>
auto Archetypes::get_or_add_next_archetype(ArchetypeId archetype_id)
    -> ArchetypeId {
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
auto Archetypes::get_or_add_prev_archetype(ArchetypeId archetype_id)
    -> ArchetypeId {
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

template <typename T>
void Archetypes::add_edge(ArchetypeId archetype_id,
                          ArchetypeId next_archetype_id) {
  archetypes_[archetype_id].add_next_edge<T>(next_archetype_id);
  archetypes_[next_archetype_id].add_prev_edge<T>(archetype_id);
}

#endif
