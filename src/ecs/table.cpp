
#include "ecs/table.h"

void Table::add_column(ComponentId component_id, Column column) {
  columns_[component_id] = std::move(column);
}

auto Table::has_column(ComponentId component_id) const -> bool {
  return columns_.find(component_id) != columns_.end();
}

auto Table::get_column_unchecked(ComponentId component_id) -> Column& {
  return columns_[component_id];
}
