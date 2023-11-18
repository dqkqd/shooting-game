#ifndef ECS_QUERY_H
#define ECS_QUERY_H

#include "ecs/world.h"

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
  explicit Query(World& world) : world_{world} {
    archetypes_ = std::move(world_.archetypes().finder().get<Args...>());
    fetch_iter();
  }

  [[nodiscard]] auto done() const -> bool {
    return archetype_index_ == archetypes_.size() - 1 && table_iter_.done();
  }

  auto next() -> std::tuple<Args&...> {
    if (!table_iter_.done()) {
      auto current_iter = table_iter_;
      ++table_iter_;
      return *current_iter;
    }

    ++archetype_index_;
    fetch_iter();
    return *table_iter_;
  }

 private:
  size_t archetype_index_{};
  Table::Iterator<Args...> table_iter_;
  std::vector<ArchetypeId> archetypes_;
  World& world_;

  void reset() {
    archetype_index_ = 0;
    fetch_iter();
  }

  void fetch_iter() {
    table_iter_ = world_.archetypes()
                      .get_by_id_unchecked(archetypes_[archetype_index_])
                      .table()
                      .begin<Args...>();
  }
};

#endif
