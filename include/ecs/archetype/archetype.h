#ifndef ECS_ARCHETYPE_ARCHETYPE_H
#define ECS_ARCHETYPE_ARCHETYPE_H

#include "ecs/archetype/edges.h"
#include "ecs/component.h"
#include "ecs/entity.h"
#include "ecs/table/table.h"

class Archetype {
 public:
  Archetype(const Archetype &) = delete;
  Archetype(Archetype &&) noexcept;
  auto operator=(const Archetype &) -> Archetype & = delete;
  auto operator=(Archetype &&) noexcept -> Archetype &;
  ~Archetype() = default;

  [[nodiscard]] auto archetype_id() const -> ArchetypeId;
  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto components() const -> const Components &;
  [[nodiscard]] auto table() -> Table &;
  [[nodiscard]] auto is_empty() const -> bool;
  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto location(EntityId entity_id) const
      -> std::optional<EntityLocation>;

  void set_archetype_id(ArchetypeId archetype_id);

  template <typename T, typename... Args>
  auto has_components() const -> bool;

  /* factory */
  template <typename... Args>
  static auto create_archetype(ArchetypeId archetype_id = INVALID_ARCHETYPE_ID)
      -> Archetype;

  /* cloning */
  template <typename T>
  auto clone_with(ArchetypeId archetype_id = INVALID_ARCHETYPE_ID) const
      -> Archetype;
  template <typename T>
  auto clone_without(ArchetypeId archetype_id = INVALID_ARCHETYPE_ID) const
      -> Archetype;

  /* entities manipulation */
  template <typename T>
  [[nodiscard]] auto get_entity_data(EntityId entity_id) -> OptionalRef<T>;

  template <typename... Args>
  auto add_entity(EntityId entity_id, Args &&...components) -> EntityLocation;

  template <typename... Args>
  auto move_entity_to_other(EntityId entity_id, Archetype &other,
                            Args &&...components)
      -> std::optional<EntityLocation>;

  /* edge caching */
  template <typename T>
  void add_next_edge(ArchetypeId archetype_id);
  template <typename T>
  void add_prev_edge(ArchetypeId archetype_id);
  template <typename T>
  auto get_next_edge() const -> std::optional<ArchetypeId>;
  template <typename T>
  auto get_prev_edge() const -> std::optional<ArchetypeId>;

 private:
  ArchetypeId archetype_id_;
  Table table_;
  Components components_;
  std::unordered_map<EntityId, EntityLocation> locations_;

  ArchetypeEdges next_edges_;
  ArchetypeEdges prev_edges_;

  Archetype(ArchetypeId archetype_id, Table &&table);
};

/* put the definition here since these are template methods */

template <typename T, typename... Args>
auto Archetype::has_components() const -> bool {
  return table_.has_components<T, Args...>();
}

template <typename... Args>
auto Archetype::create_archetype(ArchetypeId archetype_id) -> Archetype {
  return Archetype(archetype_id, Table::create_table<Args...>());
}

template <typename T>
auto Archetype::clone_with(ArchetypeId archetype_id) const -> Archetype {
  return Archetype(archetype_id, table_.clone_with<T>());
}

template <typename T>
auto Archetype::clone_without(ArchetypeId archetype_id) const -> Archetype {
  return Archetype(archetype_id, table_.clone_without<T>());
}

template <typename T>
auto Archetype::get_entity_data(EntityId entity_id) -> OptionalRef<T> {
  auto it = locations_.find(entity_id);
  if (it == locations_.end()) {
    return {};
  }

  if (!table_.has_components<T>()) {
    return {};
  }

  auto [data] = table_.get_data<T>(it->second.table_row);
  return std::ref(data);
}

template <typename... Args>
auto Archetype::add_entity(EntityId entity_id, Args &&...components)
    -> EntityLocation {
  // TODO(khanhdq): made this function return optional because add_row might
  // fail
  auto table_row = table_.add_row<Args...>(std::forward<Args>(components)...);
  auto location = EntityLocation{.entity_id = entity_id,
                                 .archetype_id = archetype_id(),
                                 .table_id = table_id(),
                                 .table_row = table_row};
  locations_[entity_id] = location;
  return location;
}

template <typename... Args>
auto Archetype::move_entity_to_other(EntityId entity_id, Archetype &other,
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
  other.table_.add_data<Args...>(std::move(components)...);

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

template <typename T>
void Archetype::add_next_edge(ArchetypeId archetype_id) {
  if (archetype_id_ == archetype_id) {
    throw std::runtime_error(fmt::format(
        "Can not add self reference edge to archetype `{}`", archetype_id));
  }
  next_edges_.add<T>(archetype_id);
}

template <typename T>
void Archetype::add_prev_edge(ArchetypeId archetype_id) {
  if (this->archetype_id_ == archetype_id) {
    throw std::runtime_error(fmt::format(
        "Can not add self reference edge to archetype `{}`", archetype_id));
  }
  prev_edges_.add<T>(archetype_id);
}

template <typename T>
auto Archetype::get_next_edge() const -> std::optional<ArchetypeId> {
  return next_edges_.get<T>();
}
template <typename T>
auto Archetype::get_prev_edge() const -> std::optional<ArchetypeId> {
  return prev_edges_.get<T>();
}

#endif
