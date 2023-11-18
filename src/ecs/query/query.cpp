#pragma once

#include "ecs/query/query.h"

template <typename... Args>
Query<Args...>::Query(Query&& query) noexcept
    : table_iter_{std::move(query.table_iter_)},
      archetype_index_{std::exchange(query.archetype_index_, 0)},
      matched_archetypes_{std::move(query.matched_archetypes_)},
      archetypes_{query.archetypes_} {}

template <typename... Args>
auto Query<Args...>::operator=(Query&& query) noexcept -> Query& {
  table_iter_ = std::move(query.table_iter_);
  archetype_index_ = std::exchange(query.archetype_index_, 0);
  matched_archetypes_ = std::move(query.matched_archetypes_);
  archetypes_ = query.archetypes_;
}

template <typename... Args>
Query<Args...>::Query(Archetypes& archetypes)
    : archetypes_{archetypes},
      matched_archetypes_{std::move(archetypes.finder().get<Args...>())} {
  fetch_iter();
}

template <typename... Args>
auto Query<Args...>::done() const -> bool {
  if (archetype_index_ >= matched_archetypes_.size()) {
    return true;
  }
  return archetype_index_ == matched_archetypes_.size() - 1 &&
         table_iter_.done();
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
  auto archetype_id = matched_archetypes_[archetype_index_];
  table_iter_ =
      archetypes_.get_by_id_unchecked(archetype_id).table().begin<Args...>();
}
