#ifndef ECS_QUERY_ITERATOR_H
#define ECS_QUERY_ITERATOR_H

#include "ecs/archetype/archetypes.h"

template <typename... Args>
class QueryIterator {
 public:
  QueryIterator(Archetypes* archetypes,
                std::vector<ArchetypeId>& matched_archetypes)
      : archetypes_{archetypes}, matched_archetypes_{matched_archetypes} {
    fetch_iter();
  }
  ~QueryIterator() = default;

  QueryIterator(const QueryIterator&) = delete;
  QueryIterator(QueryIterator&& query_iterator) = delete;
  auto operator=(const QueryIterator&) -> QueryIterator& = delete;
  auto operator=(QueryIterator&&) -> QueryIterator& = delete;

  auto next() -> std::tuple<Args&...> {
    if (table_iter_.done()) {
      ++archetype_index_;
      fetch_iter();
    }

    auto current_iter = table_iter_;
    ++table_iter_;
    return *current_iter;
  }

  [[nodiscard]] auto done() const -> bool {
    if (archetype_index_ >= matched_archetypes_.size()) {
      return true;
    }
    return archetype_index_ == matched_archetypes_.size() - 1 &&
           table_iter_.done();
  }

 private:
  std::size_t archetype_index_{};
  std::vector<ArchetypeId>& matched_archetypes_;
  Archetypes* archetypes_;
  Table::Iterator<Args...> table_iter_;

  void reset() {
    archetype_index_ = 0;
    fetch_iter();
  }

  void fetch_iter() {
    auto archetype_id = matched_archetypes_[archetype_index_];
    table_iter_ =
        archetypes_->get_by_id_unchecked(archetype_id).table().begin<Args...>();
  }
};

#endif
