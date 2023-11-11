#ifndef ECS_ARCHETYPE_H
#define ECS_ARCHETYPE_H

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
    auto table = Table();
    ([&] { table.add_column(Column::create_column<Args>()); }(), ...);
    return Archetype(std::move(table));
  }

  [[nodiscard]] auto archetype_id() const -> ArchetypeId;
  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_empty() const -> bool;

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

  template <typename... Args>
  auto add_entity(Args &&...components) -> EntityLocation {
    auto row = table_.add_row<Args...>(std::forward<Args>(components)...);
    locations_.emplace_back<EntityLocation>(
        {.archetype_id = archetype_id(), .table_id = table_id(), .row = row});
    return locations_.back();
  }

 private:
  explicit Archetype(Table &&table);

  ArchetypeId archetype_id_;
  Table table_;
  std::vector<EntityLocation> locations_;
  std::vector<ComponentId> components_;
};

#endif
