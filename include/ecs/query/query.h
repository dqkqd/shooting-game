#ifndef ECS_QUERY_QUERY_H
#define ECS_QUERY_QUERY_H

#include "ecs/query/iterator.h"

class Query {
 public:
  Query() = delete;

  Query(Archetypes& archetypes, std::vector<ArchetypeId>&& matched_archetypes)
      : archetypes_{archetypes},
        matched_archetypes_{std::move(matched_archetypes)} {}

  ~Query() = default;

  Query(const Query&) = delete;
  auto operator=(const Query&) -> Query& = delete;

  Query(Query&& query) noexcept
      : matched_archetypes_{std::move(query.matched_archetypes_)},
        archetypes_{query.archetypes_} {}

  auto operator=(Query&& query) noexcept -> Query& = delete;

  template <typename... Args>
  auto iter() -> QueryIteratorWrapper<Args...> {
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

#endif
