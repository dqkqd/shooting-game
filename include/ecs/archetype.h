#ifndef ECS_ARCHETYPE_H
#define ECS_ARCHETYPE_H

#include <vector>

#include "ecs/column.h"
#include "ecs/primitive.h"
#include "ecs/table.h"

struct ArchetypeEntity {
  EntityId entity;
  size_t row;
};

class Archetype;
using ArchetypeCounter = InstanceCounter<Archetype>;

class Archetype {
 public:
  explicit Archetype(Table &&table);

  auto archetype_id() const -> ArchetypeId;
  auto table_id() const -> TableId;

  template <typename T, typename... Args>
  auto has_components() -> bool {
    return table_.has_components<T, Args...>();
  }

  [[nodiscard]] auto components() const -> std::vector<ComponentId>;

  [[nodiscard]] auto contains(ComponentId component_id) const -> bool;
  template <class T>
  [[nodiscard]] auto contains() const -> bool {
    return contains(ComponentCounter::id<T>());
  }

 private:
  ArchetypeId archetype_id_;
  Table table_;
  std::vector<ComponentId> components_;
};

#endif
