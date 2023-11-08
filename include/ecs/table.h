#ifndef ECS_TABLE_H
#define ECS_TABLE_H

#include <unordered_map>

#include "ecs/column.h"
#include "ecs/primitive.h"

class Table {
 public:
  Table() = default;

  void add_column(ComponentId component_id, Column column);
  [[nodiscard]] auto has_column(ComponentId component_id) const -> bool;
  auto get_column_unchecked(ComponentId component_id) -> Column&;

  template <class T>
  auto get_data_unchecked(ComponentId component_id, size_t row) -> T& {
    return columns_[component_id].get_data_unchecked<T>(row);
  }

 private:
  std::unordered_map<ComponentId, Column> columns_{};
};

#endif