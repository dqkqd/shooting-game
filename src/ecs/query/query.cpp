#include "ecs/query/query.h"

Query::Query(Query&& query) noexcept
    : matched_archetypes_{std::move(query.matched_archetypes_)},
      archetypes_{query.archetypes_} {}

auto Query::operator=(Query&& query) noexcept -> Query& {
  matched_archetypes_ = std::move(query.matched_archetypes_);
  archetypes_ = query.archetypes_;
  return *this;
}

Query::Query(Archetypes* archetypes,
             std::vector<ArchetypeId>&& matched_archetypes)
    : archetypes_{archetypes},
      matched_archetypes_{std::move(matched_archetypes)} {}
