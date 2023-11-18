#ifndef ECS_QUERY_QUERY_H
#define ECS_QUERY_QUERY_H

#include "ecs/query/iterator.h"

class Query {
 public:
  explicit Query(Archetypes* archetypes,
                 std::vector<ArchetypeId>&& matched_archetypes);
  ~Query() = default;

  Query(const Query&) = delete;
  Query(Query&& /*query*/) noexcept;
  auto operator=(const Query&) -> Query& = delete;
  auto operator=(Query&& /*query*/) noexcept -> Query&;

  template <typename... Args>
  auto iter() -> QueryIterator<Args...> {
    return QueryIterator<Args...>(archetypes_, matched_archetypes_);
  }

 private:
  std::vector<ArchetypeId> matched_archetypes_{};
  // TODO(khanhdq): make this shared ptr
  Archetypes* archetypes_;

  void reset();
};

#endif
