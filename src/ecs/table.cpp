
#include "ecs/table.h"

#include "ecs/primitive.h"

Table::Table(TableId table) : table_id_{table} {}

auto Table::create_table() -> Table { return Table(TableCounter::id()); }

auto Table::table_id() const -> TableId { return table_id_; }

void Table::add_column(Column&& column) {
  columns_[column.component_id()] = std::move(column);
}

auto Table::has_component(ComponentId component_id) const -> bool {
  return columns_.find(component_id) != columns_.end();
}

auto Table::get_column_unchecked(ComponentId component_id) -> Column& {
  return columns_[component_id];
}
