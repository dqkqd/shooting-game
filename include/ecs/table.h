#ifndef ECS_TABLE_H
#define ECS_TABLE_H

#include <unordered_map>

#include "ecs/column.h"
#include "ecs/primitive.h"

class Table;
using TableCounter = Counter<Table>;

class Table {
 public:
  explicit Table(TableId table = INVALID_TABLE_ID);

  void add_column(Column&& column);

  template <class T>
  [[nodiscard]] auto has_component() const -> bool {
    return has_component(ComponentCounter::id<T>());
  }
  [[nodiscard]] auto has_component(ComponentId component_id) const -> bool;

  auto get_column_unchecked(ComponentId component_id) -> Column&;

  template <class T>
  auto get_data_unchecked(ComponentId component_id, size_t row) -> T& {
    return columns_[component_id].get_data_unchecked<T>(row);
  }

 private:
  TableId table_id_;
  std::unordered_map<ComponentId, Column> columns_{};
};

#endif