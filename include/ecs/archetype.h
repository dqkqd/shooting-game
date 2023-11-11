#ifndef ECS_ARCHETYPE_H
#define ECS_ARCHETYPE_H

#include <optional>
#include <stdexcept>
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
  [[nodiscard]] auto is_valid() const -> bool;

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

  template <typename... Args>
  auto move_entity_to_other(EntityId entity_id, Archetype &other,
                            Args &&...components)
      -> std::optional<EntityLocation> {
    auto entity_location = location(entity_id);
    if (!entity_location.has_value()) {
      return {};
    }

    if (other.location(entity_id).has_value()) {
      std::ostringstream error_msg;
      error_msg << "Archetype " << other.archetype_id()
                << " already has entity " << entity_id << std::endl;
      throw std::runtime_error(error_msg.str().c_str());
    }

    auto old_height = other.table_.height();
    table_.move_row_to_other(entity_location->row, other.table_);
    if (sizeof...(Args) > 0) {
      other.table_.add_components<Args...>(std::move(components)...);
    }

    locations_.erase(entity_id);
    other.table_.reset_height();
    auto new_height = other.table_.height();

    if (!other.is_valid() || old_height + 1 != new_height) {
      std::ostringstream error_msg;
      error_msg << "Could move entity " << entity_id << " from archetype "
                << archetype_id_ << " into archetype " << other.archetype_id()
                << std::endl;
      throw std::runtime_error(error_msg.str().c_str());
    }

    if (other.is_empty()) {
      return {};
    }

    auto location = EntityLocation{.archetype_id = other.archetype_id(),
                                   .table_id = other.table_id(),
                                   .row = other.table_.height() - 1};
    other.locations_[entity_id] = location;
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
