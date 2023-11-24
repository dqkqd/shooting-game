
#include "ecs/table/table.h"

#include <limits>
#include <stdexcept>
#include <utility>

#include "ecs/component.h"
#include "ecs/primitive.h"

Table::Table() : table_id_{TableCounter::id()}, width_{0}, height_{0} {}

Table::Table(Table&& table) noexcept
    : table_id_{std::exchange(table.table_id_, INVALID_TABLE_ID)},
      width_{std::exchange(table.width_, {})},
      height_{std::exchange(table.height_, {})},
      components_{std::move(table.components_)},
      columns_{std::exchange(table.columns_, {})} {}

auto Table::operator=(Table&& table) noexcept -> Table& {
  table_id_ = std::exchange(table.table_id_, INVALID_TABLE_ID);
  components_ = std::move(table.components_);
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

auto Table::components() const -> const Components& { return components_; }

auto Table::get_column(ComponentId component_id) -> Column& {
  auto it = columns_.find(component_id);
  if (it == columns_.end()) {
    throw std::runtime_error(fmt::format(
        "Column with component id `{}` does not exist", component_id));
  }
  return it->second;
}

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

auto Table::move_row_to_other(size_t row, Table& other)
    -> std::optional<size_t> {
  if (row >= height_) {
    return {};
  }

  for (auto& [component_id, column] : columns_) {
    if (other.components_.has_component(component_id)) {
      other.get_column(component_id).push_from(column, row);
    }
  }
  other.reset_height();
  remove_row(row);
  return other.height() - 1;
}

void Table::reset_height() {
  all_heights_equal_ = true;
  if (columns_.empty()) {
    return;
  }

  size_t min_height = std::numeric_limits<size_t>::max();
  size_t max_height = std::numeric_limits<size_t>::min();

  for (auto& [_, column] : columns_) {
    min_height = std::min(min_height, column.size());
    max_height = std::max(max_height, column.size());
    if (min_height < max_height) {
      all_heights_equal_ = false;
      break;
    }
  }

  height_ = min_height;
}

void Table::add_column(ComponentId component_id, Column&& column) {
  columns_.emplace(component_id, std::move(column));
  ++width_;
}
