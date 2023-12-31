#ifndef ECS_QUERY_QUERY_H
#define ECS_QUERY_QUERY_H

#include "ecs/observer/event.h"
#include "ecs/observer/observer.h"
#include "ecs/query/iterator.h"

template <class... Args>
class Query {
  static_assert(all_types_are_different<Args...>());

 public:
  Query(QueryIterator<Args...>&& begin, QueryIterator<Args...>&& end)
      : begin_{std::move(begin)}, end_{std::move(end)} {}

  Query(const Query&) = delete;
  auto operator=(const Query&) -> Query& = delete;

  Query(Query&& query) noexcept
      : begin_{std::move(query.begin_)}, end_{std::move(query.end_)} {};
  auto operator=(Query&& query) noexcept -> Query& {
    begin_ = std::move(query.begin_);
    end_ = std::move(query.end_);
  };

  ~Query() = default;

  auto begin() -> QueryIterator<Args...> { return std::move(begin_); }
  auto end() -> QueryIterator<Args...> { return std::move(end_); }

 private:
  QueryIterator<Args...> begin_;
  QueryIterator<Args...> end_;
};

class QueryWrapper {
 public:
  QueryWrapper() = delete;

  explicit QueryWrapper(std::vector<ArchetypeId>&& matched_archetypes)
      : matched_archetypes_{std::move(matched_archetypes)} {}

  ~QueryWrapper() = default;

  QueryWrapper(const QueryWrapper&) = delete;
  auto operator=(const QueryWrapper&) -> QueryWrapper& = delete;

  QueryWrapper(QueryWrapper&& query) noexcept
      : matched_archetypes_{std::move(query.matched_archetypes_)} {}

  auto operator=(QueryWrapper&& query) noexcept -> QueryWrapper& = delete;

  template <typename... Args>
  auto query(Archetypes& archetypes) -> Query<Args...> {
    return {begin<Args...>(archetypes), end<Args...>(archetypes)};
  }

  template <typename... Args>
  auto begin(Archetypes& archetypes) -> QueryIterator<Args...> {
    return QueryIterator<Args...>::create_begin(archetypes,
                                                matched_archetypes_);
  }

  template <typename... Args>
  auto end(Archetypes& archetypes) -> QueryIterator<Args...> {
    return QueryIterator<Args...>::create_end(archetypes, matched_archetypes_);
  }

  void add_archetype(ArchetypeId archetype_id) {
    matched_archetypes_.emplace_back(archetype_id);
  }

 private:
  std::vector<ArchetypeId> matched_archetypes_{};
};

class Queries : public EventListener {
 public:
  [[nodiscard]] auto size() const -> std::size_t { return queries_.size(); };

  template <typename... Args>
  auto get(Archetypes& archetypes) -> Query<Args...> {
    auto components = Components::from_types<Args...>();
    auto it = queries_.find(components);
    if (it != queries_.end()) {
      return it->second.template query<Args...>(archetypes);
    }
    auto [added_it, inserted] = queries_.emplace(
        std::move(components), QueryWrapper(archetypes.find<Args...>()));
    return added_it->second.template query<Args...>(archetypes);
  }

  void on_notify(Event* event) final {
    switch (event->type()) {
      case EventType::ADD_ARCHETYPE: {
        auto* e = event->as<AddArchetypeEvent>();
        for (auto& [components, query] : queries_) {
          if (e->components().cover(components)) {
            query.add_archetype(e->archetype_id());
          }
        }
      }
    }
  }

 private:
  std::map<Components, QueryWrapper> queries_;
};

#endif
