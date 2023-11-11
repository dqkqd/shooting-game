#ifndef ECS_ARCHETYPE_H
#define ECS_ARCHETYPE_H

#include <optional>
#include <unordered_map>
#include <vector>

#include "ecs/column.h"
#include "ecs/primitive.h"
#include "ecs/table.h"

struct EntityLocation {
  ArchetypeId archetype_id;
  TableId table_id;
  size_t row;
};

class Archetype;
using ArchetypeCounter = InstanceCounter<Archetype>;

class Archetype {
 public:
  Archetype(const Archetype &) = delete;
  Archetype(Archetype &&) noexcept;
  auto operator=(const Archetype &) -> Archetype & = delete;
  auto operator=(Archetype &&) noexcept -> Archetype &;
  ~Archetype() = default;

  template <typename... Args>
  static auto create_archetype() -> Archetype {
    return Archetype(Table::create_table<Args...>());
  }

  [[nodiscard]] auto archetype_id() const -> ArchetypeId;
  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_empty() const -> bool;

  template <typename T, typename... Args>
  auto has_components() -> bool {
    return table_.has_components<T, Args...>();
  }

  [[nodiscard]] auto components() const -> std::vector<ComponentId>;

  template <typename... Args>
  auto add_entity(EntityId entity_id, Args &&...components) -> EntityLocation {
    auto row = table_.add_row<Args...>(std::forward<Args>(components)...);
    auto location = EntityLocation{
        .archetype_id = archetype_id(), .table_id = table_id(), .row = row};
    locations_[entity_id] = location;
    return location;
  }

  [[nodiscard]] auto location(EntityId entity_id) const
      -> std::optional<EntityLocation>;

 private:
  explicit Archetype(Table &&table);

  ArchetypeId archetype_id_;
  Table table_;
  std::unordered_map<EntityId, EntityLocation> locations_;
  std::vector<ComponentId> components_;
};

#endif
