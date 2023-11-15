#include "ecs/archetype/archetypes.h"

auto Archetypes::size() const -> size_t { return archetypes_.size(); }

auto Archetypes::get_by_id_unchecked(ArchetypeId archetype_id) -> Archetype& {
  return archetypes_[archetype_id];
}
