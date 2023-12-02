#ifndef ECS_QUERY_ITERATOR_H
#define ECS_QUERY_ITERATOR_H

#include "ecs/archetype/archetypes.h"
#include "ecs/primitive.h"

template <typename... Args>
class QueryIterator {
  static_assert(all_types_are_different<Args...>());

 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::tuple<Args...>;
  using pointer = std::tuple<Args*...>;
  using reference = std::tuple<Args&...>;

  QueryIterator(Archetypes& archetypes,
                std::vector<ArchetypeId>& matched_archetypes)
      : archetypes_{archetypes}, matched_archetypes_{matched_archetypes} {
    fetch();
  }

  auto operator*() -> reference { return *table_iter_; }

  auto operator++() -> QueryIterator& {
    advance();
    return *this;
  }

  [[nodiscard]] auto done() const -> bool {
    if (archetype_index_ >= matched_archetypes_.size()) {
      return true;
    }
    return archetype_index_ == matched_archetypes_.size() - 1 &&
           table_iter_.done();
  }

  friend auto operator==(const QueryIterator& lhs, const QueryIterator& rhs)
      -> bool {
    return lhs.table_iter_ == rhs.table_iter_;
  };
  friend auto operator!=(const QueryIterator& lhs, const QueryIterator& rhs)
      -> bool {
    return lhs.table_iter_ != rhs.table_iter_;
  };

  void close() {
    if (!matched_archetypes_.empty()) {
      archetype_index_ = matched_archetypes_.size() - 1;
      auto archetype_id = matched_archetypes_[archetype_index_];
      table_iter_ =
          archetypes_.get_by_id_unchecked(archetype_id).table().end<Args...>();
    }
  }

 private:
  Archetypes& archetypes_;
  std::vector<ArchetypeId>& matched_archetypes_;
  TableIterator<Args...> table_iter_;
  std::size_t archetype_index_{};

  void fetch() {
    if (archetype_index_ < matched_archetypes_.size()) {
      auto archetype_id = matched_archetypes_[archetype_index_];
      table_iter_ = archetypes_.get_by_id_unchecked(archetype_id)
                        .table()
                        .begin<Args...>();
    }
  }

  void advance() {
    ++table_iter_;
    if (table_iter_.done()) {
      ++archetype_index_;
      fetch();
    }
  }
};

#endif
