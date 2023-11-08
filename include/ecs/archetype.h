#ifndef ECS_ARCHETYPE_H
#define ECS_ARCHETYPE_H

#include <cstddef>
#include <memory>

class Archetype {};

struct ArchetypeRecord {
  std::shared_ptr<Archetype> archetype;
  size_t entity_row;
};

#endif
