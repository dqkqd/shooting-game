#ifndef ECS_ARCHETYPE_H
#define ECS_ARCHETYPE_H

#include <vector>

#include "ecs/primitive.h"
#include "ecs/table.h"

struct ArchetypeEntity {
  EntityId entity;
  size_t row;
};

class Archetype;
using ArchetypeCounter = Counter<Archetype>;

class Archetype {
 public:
  explicit Archetype(TableId table) : table_(table) {}
  [[nodiscard]] auto entities() const -> std::vector<ArchetypeEntity> &;
  [[nodiscard]] auto components() const -> std::vector<ComponentId> &;
  [[nodiscard]] auto contains(ComponentId component) const -> bool;

 private:
  std::vector<ComponentId> components_;
  std::vector<ArchetypeEntity> entities_;
  ArchetypeId id_ = INVALID_ARCHETYPE_ID;
  TableId table_;
};

#endif
