
#include "ecs/table.h"

#include <utility>

#include "ecs/primitive.h"

Table::Table() : table_id_{TableCounter::id()} {}

Table::Table(Table&& table) noexcept
    : table_id_{std::exchange(table.table_id_, INVALID_TABLE_ID)},
      columns_{std::exchange(table.columns_, {})} {}

auto Table::operator=(Table&& table) noexcept -> Table& {
  table_id_ = std::exchange(table.table_id_, INVALID_TABLE_ID);
  columns_ = std::exchange(table.columns_, {});
  return *this;
};

auto Table::create_table() -> Table { return Table(); }

auto Table::table_id() const -> TableId { return table_id_; }
auto Table::is_valid() const -> bool { return table_id_ != INVALID_TABLE_ID; }

void Table::add_column(Column&& column) {
  columns_[column.component_id()] = std::move(column);
}

auto Table::components() const -> std::vector<ComponentId> {
  std::vector<ComponentId> component_ids;
  component_ids.reserve(columns_.size());
  for (const auto& it : columns_) {
    component_ids.push_back(it.first);
  }
  return component_ids;
}

auto Table::has_component_id(ComponentId component_id) const -> bool {
  return columns_.find(component_id) != columns_.end();
}

auto Table::get_column_unchecked(ComponentId component_id) -> Column& {
  return columns_[component_id];
}
