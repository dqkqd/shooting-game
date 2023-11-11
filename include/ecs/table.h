#ifndef ECS_TABLE_H
#define ECS_TABLE_H

#include <type_traits>
#include <unordered_map>
#include <vector>

#include "ecs/column.h"
#include "ecs/primitive.h"

class Table;
using TableCounter = InstanceCounter<Table>;

class Table {
 public:
  explicit Table();

  Table(const Table &) = delete;
  Table(Table &&Table) noexcept;
  auto operator=(const Table &) -> Table & = delete;
  auto operator=(Table &&table) noexcept -> Table &;
  ~Table() = default;

  static auto create_table() -> Table;

  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_valid() const -> bool;

  void add_column(Column &&column);
  [[nodiscard]] auto components() const -> std::vector<ComponentId>;

  template <class T>
  [[nodiscard]] auto has_component() const -> bool {
    return has_component(ComponentCounter::id<T>());
  }
  [[nodiscard]] auto has_component(ComponentId component_id) const -> bool;

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  auto has_components(ComponentId component_id, Args... component_ids) -> bool {
    return has_component(component_id) && (... && has_component(component_ids));
  }
  template <typename T, typename... Args>
  auto has_components() -> bool {
    return has_component<T>() && (... && has_component<Args>());
  }

  template <class T>
  auto get_column_unchecked() -> Column & {
    return get_column_unchecked(ComponentCounter::id<T>());
  }
  auto get_column_unchecked(ComponentId component_id) -> Column &;

  template <class T>
  auto get_data_unchecked(size_t row) -> T & {
    return columns_[ComponentCounter::id<T>()].template get_data_unchecked<T>(
        row);
  }

 private:
  TableId table_id_;
  std::unordered_map<ComponentId, Column> columns_{};
};

#endif