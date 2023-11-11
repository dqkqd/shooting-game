
#include "ecs/table.h"

#include <stdexcept>
#include <utility>

#include "ecs/primitive.h"

Table::Table() : table_id_{TableCounter::id()}, width_{0}, height_{0} {}

Table::Table(Table&& table) noexcept
    : table_id_{std::exchange(table.table_id_, INVALID_TABLE_ID)},
      columns_{std::exchange(table.columns_, {})},
      width_{std::exchange(table.width_, {})},
      height_{std::exchange(table.height_, {})} {}

auto Table::operator=(Table&& table) noexcept -> Table& {
  table_id_ = std::exchange(table.table_id_, INVALID_TABLE_ID);
  columns_ = std::exchange(table.columns_, {});
  width_ = std::exchange(table.width_, {});
  height_ = std::exchange(table.height_, {});
  return *this;
};

auto Table::create_table() -> Table { return Table(); }
auto Table::table_id() const -> TableId { return table_id_; }
auto Table::is_valid() const -> bool { return table_id_ != INVALID_TABLE_ID; }
auto Table::is_empty() const -> bool { return height_ == 0; }
auto Table::width() const -> size_t { return width_; }
auto Table::height() const -> size_t { return height_; }

void Table::add_column(Column&& column) {
  if (!is_empty() && column.size() != height_) {
    throw std::runtime_error("Can not add column into table. Size mismatch.");
  }
  height_ = column.size();

  if (!has_component_id(column.component_id())) {
    columns_[column.component_id()] = std::move(column);
    ++width_;
  }
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
