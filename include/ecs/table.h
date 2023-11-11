#ifndef ECS_TABLE_H
#define ECS_TABLE_H

#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "ecs/column.h"
#include "ecs/primitive.h"

class Table;
using TableCounter = InstanceCounter<Table>;

class Table {
 public:
  Table();

  Table(const Table &) = delete;
  Table(Table &&Table) noexcept;
  auto operator=(const Table &) -> Table & = delete;
  auto operator=(Table &&table) noexcept -> Table &;
  ~Table() = default;

  template <typename... Args>
  static auto create_table() -> Table {
    static_assert(sizeof...(Args) > 0, "Can not create table without column");
    static_assert(all_types_are_different<Args...>(),
                  "All column types must be pairwise different");

    auto table = Table();
    table.width_ = sizeof...(Args);
    (
        [&] {
          table.columns_[ComponentCounter::id<Args>()] =
              Column::create_column<Args>();
        }(),
        ...);
    return table;
  }

  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto is_empty() const -> bool;
  [[nodiscard]] auto width() const -> size_t;
  [[nodiscard]] auto height() const -> size_t;

  [[nodiscard]] auto components() const -> std::vector<ComponentId>;

  [[nodiscard]] auto has_component_id(ComponentId component_id) const -> bool;

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  auto has_components(ComponentId component_id, Args... component_ids) -> bool {
    return has_component_id(component_id) &&
           (... && has_component_id(component_ids));
  }
  template <typename T, typename... Args>
  auto has_components() -> bool {
    return has_components(ComponentCounter::id<T>(),
                          ComponentCounter::id<Args>()...);
  }

  template <class T>
  auto get_column() -> Column & {
    return get_column(ComponentCounter::id<T>());
  }
  auto get_column(ComponentId component_id) -> Column &;

  template <class T>
  auto get_data_unchecked(size_t row) -> T & {
    return get_column<T>().template get_data_unchecked<T>(row);
  }

  template <typename... Args>
  auto add_row(Args &&...components) -> size_t {
    if (sizeof...(components) != width_) {
      std::ostringstream error_msg;
      error_msg << "Could not add row into table. "
                << "Rows: " << sizeof...(components) << ", width: " << width_
                << std::endl;
      throw std::runtime_error(error_msg.str());
    }
    static_assert(all_types_are_different<Args...>(),
                  "All the types in must be pairwise different");
    (
        [&] {
          Column &column = get_column<Args>();
          column.push<Args>(std::move(components));
        }(),
        ...);
    ++height_;
    size_t row_index = height_ - 1;
    return row_index;
  }

  auto remove_row(size_t row) -> bool;

 private:
  TableId table_id_;
  std::unordered_map<ComponentId, Column> columns_{};

  size_t width_;
  size_t height_;
};

#endif