#ifndef ECS_QUERY_H
#define ECS_QUERY_H

#include <tuple>

#include "ecs/table.h"

class World;

class BaseQuery {
 public:
  explicit BaseQuery(World& world);
  BaseQuery(const BaseQuery&) = default;
  BaseQuery(BaseQuery&&) = delete;
  auto operator=(const BaseQuery&) -> BaseQuery& = delete;
  auto operator=(BaseQuery&&) -> BaseQuery& = delete;
  virtual ~BaseQuery();
};

template <typename... Args>
class Query {
 public:
  explicit Query(World& world);
  [[nodiscard]] auto done() const -> bool;
  auto next() -> std::tuple<Args&...>;

 private:
  std::size_t archetype_index_{};
  Table::Iterator<Args...> table_iter_;
  std::vector<ArchetypeId> archetypes_;
  World& world_;

  void reset();
  void fetch_iter();
};

#include "../../src/ecs/query.cpp"

#endif
