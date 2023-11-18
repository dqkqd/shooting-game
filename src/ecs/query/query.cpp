#pragma once

#include "ecs/query/query.h"

#include "ecs/world.h"

template <typename... Args>
Query<Args...>::Query(World& world) : world_{world} {
  archetypes_ = std::move(world_.archetypes().finder().get<Args...>());
  fetch_iter();
}

template <typename... Args>
auto Query<Args...>::done() const -> bool {
  return archetype_index_ == archetypes_.size() - 1 && table_iter_.done();
}

template <typename... Args>
auto Query<Args...>::next() -> std::tuple<Args&...> {
  if (table_iter_.done()) {
    ++archetype_index_;
    fetch_iter();
  }

  auto current_iter = table_iter_;
  ++table_iter_;
  return *current_iter;
}
template <typename... Args>
void Query<Args...>::reset() {
  archetype_index_ = 0;
  fetch_iter();
}

template <typename... Args>
void Query<Args...>::fetch_iter() {
  table_iter_ = world_.archetypes()
                    .get_by_id_unchecked(archetypes_[archetype_index_])
                    .table()
                    .begin<Args...>();
}
