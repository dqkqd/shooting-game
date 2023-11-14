#ifndef ECS_ARCHETYPE_ARCHETYPE_H
#define ECS_ARCHETYPE_ARCHETYPE_H

#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "ecs/archetype/archetype_components.h"
#include "ecs/archetype/archetype_edges.h"
#include "ecs/column.h"
#include "ecs/primitive.h"
#include "ecs/table.h"

struct EntityLocation {
  EntityId entity_id;
  ArchetypeId archetype_id;
  TableId table_id;
  size_t table_row;
};

class Archetype {
 public:
  Archetype(const Archetype &) = delete;
  Archetype(Archetype &&) noexcept;
  auto operator=(const Archetype &) -> Archetype & = delete;
  auto operator=(Archetype &&) noexcept -> Archetype &;
  ~Archetype() = default;

  template <typename... Args>
  static auto create_archetype(ArchetypeId archetype_id) -> Archetype {
    return Archetype(archetype_id, Table::create_table<Args...>());
  }

  template <typename T>
  auto clone_with(ArchetypeId archetype_id) const -> Archetype {
    return Archetype(archetype_id, table_.clone_with<T>());
  }
  template <typename T>
  auto clone_without(ArchetypeId archetype_id) const -> Archetype {
    return Archetype(archetype_id, table_.clone_without<T>());
  }

  [[nodiscard]] auto archetype_id() const -> ArchetypeId;
  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_empty() const -> bool;
  [[nodiscard]] auto is_valid() const -> bool;

  template <typename T, typename... Args>
  auto has_components() -> bool {
    return table_.has_components<T, Args...>();
  }

  [[nodiscard]] auto components() const -> ArchetypeComponents;

  template <typename... Args>
  auto add_entity(EntityId entity_id, Args &&...components) -> EntityLocation {
    auto table_row = table_.add_row<Args...>(std::forward<Args>(components)...);
    auto location = EntityLocation{.entity_id = entity_id,
                                   .archetype_id = archetype_id(),
                                   .table_id = table_id(),
                                   .table_row = table_row};
    locations_[entity_id] = location;
    return location;
  }

  template <typename... Args>
  auto move_entity_to_other(EntityId entity_id, Archetype &other,
                            Args &&...components)
      -> std::optional<EntityLocation> {
    if (other.location(entity_id).has_value()) {
      spdlog::error("Entity {} has already existed in archetype {}", entity_id,
                    other.archetype_id_);
      return {};
    }

    auto entity_location = location(entity_id);
    if (!entity_location.has_value()) {
      spdlog::error("Entity {} does not exist in archetype {}", entity_id,
                    archetype_id_);
      return {};
    }

    auto old_height = other.table_.height();
    // Construct new row in other's table
    auto new_row =
        table_.move_row_to_other(entity_location->table_row, other.table_);
    if (!new_row.has_value()) {
      spdlog::error("Moving invalid entity {}", entity_id);
      return {};
    }

    // Adding missing components
    if (sizeof...(Args) > 0) {
      other.table_.add_components<Args...>(std::move(components)...);
    }

    other.table_.reset_height();
    auto new_height = other.table_.height();

    if (!other.is_valid() || old_height + 1 != new_height) {
      // Since we changed archetype's table structure, this can't be undone,
      // hence we should throw error here instead of return nullopt
      throw std::runtime_error(fmt::format(
          "Can not move entity `{}` from archetype `{}` into archetype `{}`",
          entity_id, archetype_id_, other.archetype_id_));
    }

    if (other.is_empty()) {
      return {};
    }

    auto location = EntityLocation{.entity_id = entity_id,
                                   .archetype_id = other.archetype_id(),
                                   .table_id = other.table_id(),
                                   .table_row = old_height};
    locations_.erase(entity_id);
    other.locations_[entity_id] = location;
    return location;
  }

  [[nodiscard]] auto location(EntityId entity_id) const
      -> std::optional<EntityLocation>;

  template <typename T>
  void add_next_edge(Archetype &archetype) {
    if (this->archetype_id_ == archetype.archetype_id_) {
      throw std::runtime_error(fmt::format(
          "Can not add self reference edge to archetype `{}`", archetype_id_));
    }
    next_edges_.add<T>(archetype.archetype_id());
    archetype.prev_edges_.add<T>(this->archetype_id());
  }
  template <typename T>
  void add_prev_edge(Archetype &archetype) {
    if (this->archetype_id_ == archetype.archetype_id_) {
      throw std::runtime_error(fmt::format(
          "Can not add self reference edge to archetype `{}`", archetype_id_));
    }
    prev_edges_.add<T>(archetype.archetype_id());
    archetype.next_edges_.add<T>(this->archetype_id());
  }
  template <typename T>
  auto get_next_edge() -> std::optional<ArchetypeId> {
    return next_edges_.get<T>();
  }
  template <typename T>
  auto get_prev_edge() -> std::optional<ArchetypeId> {
    return prev_edges_.get<T>();
  }

 private:
  Archetype(ArchetypeId archetype_id, Table &&table);

  ArchetypeId archetype_id_;
  Table table_;
  std::unordered_map<EntityId, EntityLocation> locations_;
  ArchetypeEdges next_edges_;
  ArchetypeEdges prev_edges_;
};

#endif
