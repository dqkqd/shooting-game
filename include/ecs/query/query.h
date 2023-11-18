#ifndef ECS_QUERY_QUERY_H
#define ECS_QUERY_QUERY_H

#include <tuple>

#include "ecs/query/base.h"
#include "ecs/table.h"

class World;

template <typename... Args>
class Query : public BaseQuery {
 public:
  explicit Query(World& world);
  ~Query() override = default;

  Query(const Query&) = delete;
  Query(Query&& /*query*/) noexcept;
  auto operator=(const Query&) -> Query& = delete;
  auto operator=(Query&& /*query*/) noexcept -> Query&;

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

#include "../../../src/ecs/query/query.cpp"

#endif
