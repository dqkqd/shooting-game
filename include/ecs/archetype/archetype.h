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

class Archetype;
using ArchetypeCounter = InstanceCounter<Archetype>;

struct EntityLocation {
  ArchetypeId archetype_id;
  TableId table_id;
  size_t row;
};

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

  template <typename T>
  auto clone_with() const -> Archetype {
    return Archetype(table_.clone_with<T>());
  }
  template <typename T>
  auto clone_without() const -> Archetype {
    return Archetype(table_.clone_without<T>());
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
      throw std::runtime_error(
          fmt::format("Archetype `{}` already has entity `{}`",
                      other.archetype_id_, entity_id));
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
      throw std::runtime_error(fmt::format(
          "Can not move entity `{}` from archetype `{}` into archetype `{}`",
          entity_id, archetype_id_, other.archetype_id_));
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

  template <typename T>
  void add_next_edge(Archetype &archetype) {
    if (this->archetype_id_ == archetype.archetype_id_) {
      throw std::runtime_error(fmt::format(
          "Can not add self reference edge to archetype `{}`", archetype_id_));
    }
    next_edges_.add<T>(archetype);
    archetype.prev_edges_.add<T>(*this);
  }
  template <typename T>
  void add_prev_edge(Archetype &archetype) {
    if (this->archetype_id_ == archetype.archetype_id_) {
      throw std::runtime_error(fmt::format(
          "Can not add self reference edge to archetype `{}`", archetype_id_));
    }
    prev_edges_.add<T>(archetype);
    archetype.next_edges_.add<T>(*this);
  }
  template <typename T>
  auto get_next_edge() -> OptionalRef<Archetype> {
    return next_edges_.get<T>();
  }
  template <typename T>
  auto get_prev_edge() -> OptionalRef<Archetype> {
    return prev_edges_.get<T>();
  }

 private:
  explicit Archetype(Table &&table);

  ArchetypeId archetype_id_;
  Table table_;
  std::unordered_map<EntityId, EntityLocation> locations_;
  ArchetypeEdges next_edges_;
  ArchetypeEdges prev_edges_;
};

#endif
