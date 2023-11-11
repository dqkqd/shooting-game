
#include "ecs/table.h"

#include <limits>
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

auto Table::table_id() const -> TableId { return table_id_; }
auto Table::is_valid() const -> bool {
  return table_id_ != INVALID_TABLE_ID && all_heights_equal_;
}
auto Table::is_empty() const -> bool { return height_ == 0; }
auto Table::width() const -> size_t { return width_; }
auto Table::height() const -> size_t { return height_; }

auto Table::remove_row(size_t row) -> bool {
  if (row >= height_) {
    return false;
  }
  for (auto& [_, column] : columns_) {
    column.remove(row);
  }
  --height_;
  return true;
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

auto Table::get_column(ComponentId component_id) -> Column& {
  auto it = columns_.find(component_id);
  if (it == columns_.end()) {
    std::ostringstream error_msg;
    error_msg << "Column with component id " << component_id
              << " does not exist" << std::endl;
    throw std::runtime_error(error_msg.str());
  }
  return it->second;
}

auto Table::add_component(ComponentId component_id, void* item) -> size_t {
  all_heights_equal_ = false;
  auto& column = get_column(component_id);
  column.push_unknown(item);
  return column.size();
}

auto Table::move_row_to_other(size_t row, Table& other)
    -> std::optional<size_t> {
  if (row >= height_) {
    return {};
  }

  size_t height = other.height();
  bool at_least_one_column_is_moved = false;
  for (auto& [component_id, column] : columns_) {
    if (other.has_component_id(component_id)) {
      height = other.add_component(component_id, column.get_ptr_at(row));
    }
  }

  other.reset_height();

  remove_row(row);
  return height;
}

void Table::reset_height() {
  if (columns_.empty()) {
    return;
  }

  size_t min_height = std::numeric_limits<size_t>::max();
  size_t max_height = std::numeric_limits<size_t>::min();

  for (auto& [_, column] : columns_) {
    min_height = std::min(min_height, column.size());
    max_height = std::max(max_height, column.size());
  }

  if (min_height == max_height) {
    height_ = min_height;
    all_heights_equal_ = true;
  }
}