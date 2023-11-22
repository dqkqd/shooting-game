#ifndef ECS_QUERY_QUERY_H
#define ECS_QUERY_QUERY_H

#include "ecs/query/iterator.h"

template <class... Args>
class Query {
  static_assert(all_types_are_different<Args...>());

 public:
  Query(QueryIterator<Args...> begin, QueryIterator<Args...> end)
      : begin_{begin}, end_{end} {}
  auto begin() -> QueryIterator<Args...> { return begin_; }
  auto end() -> QueryIterator<Args...> { return end_; }

 private:
  QueryIterator<Args...> begin_;
  QueryIterator<Args...> end_;
};

class QueryWrapper {
 public:
  QueryWrapper() = delete;

  QueryWrapper(Archetypes& archetypes,
               std::vector<ArchetypeId>&& matched_archetypes)
      : archetypes_{archetypes},
        matched_archetypes_{std::move(matched_archetypes)} {}

  ~QueryWrapper() = default;

  QueryWrapper(const QueryWrapper&) = delete;
  auto operator=(const QueryWrapper&) -> QueryWrapper& = delete;

  QueryWrapper(QueryWrapper&& query) noexcept
      : matched_archetypes_{std::move(query.matched_archetypes_)},
        archetypes_{query.archetypes_} {}

  auto operator=(QueryWrapper&& query) noexcept -> QueryWrapper& = delete;

  template <typename... Args>
  auto query() -> Query<Args...> {
    return {begin<Args...>(), end<Args...>()};
  }

  template <typename... Args>
  auto begin() -> QueryIterator<Args...> {
    return QueryIterator<Args...>(archetypes_, matched_archetypes_);
  }

  template <typename... Args>
  auto end() -> QueryIterator<Args...> {
    auto query = QueryIterator<Args...>(archetypes_, matched_archetypes_);
    query.close();
    return query;
  }

 private:
  std::vector<ArchetypeId> matched_archetypes_{};
  Archetypes& archetypes_;
};

class Queries {
 public:
  template <typename... Args>
  void add_query(Archetypes& archetypes) {
    auto components = Components::from_types<Args...>();
    if (queries_.count(components) > 0) {
      return;
    }
    queries_.emplace(std::move(components),
                     QueryWrapper(archetypes, archetypes.find<Args...>()));
  }

  template <typename... Args>
  auto get_query() -> Query<Args...> {
    return queries_.at(Components::from_types<Args...>())
        .template query<Args...>();
  }

 private:
  std::map<Components, QueryWrapper> queries_;
};

#endif
