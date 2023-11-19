#ifndef ECS_TABLE_TABLE_H
#define ECS_TABLE_TABLE_H

#include "ecs/component.h"
#include "ecs/primitive.h"
#include "ecs/table/iterator.h"

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

  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto is_empty() const -> bool;
  [[nodiscard]] auto width() const -> size_t;
  [[nodiscard]] auto height() const -> size_t;
  [[nodiscard]] auto components() const -> const Components &;

  /* factory */
  template <typename... Args>
  static auto create_table() -> Table;

  /* cloning */
  template <typename T>
  auto clone_with() const -> Table;
  template <typename T>
  auto clone_without() const -> Table;

  template <typename T, typename... Args>
  auto has_components() const -> bool;

  template <typename... Args>
  void add_data(Args &&...components);
  template <class T>
  auto get_data(size_t row) -> T &;  // TODO(khanhdq): remove
  template <typename... Args>
  auto get_data_row(size_t row) -> std::tuple<Args &...>;

  template <typename... Args>
  auto add_row(Args &&...components) -> size_t;
  auto remove_row(size_t row) -> bool;
  auto move_row_to_other(size_t row, Table &other) -> std::optional<size_t>;

  template <typename... Args>
  auto begin() -> TableIterator<Args...>;
  template <typename... Args>
  auto end() -> TableIterator<Args...>;

 private:
  TableId table_id_;
  std::size_t width_;
  std::size_t height_;
  Components components_{};
  std::unordered_map<ComponentId, Column> columns_{};
  bool all_heights_equal_ = true;

  void reset_height();

  void add_column(ComponentId component_id, Column &&column);
  template <class T>
  auto get_column() -> Column &;
  auto get_column(ComponentId component_id) -> Column &;

  auto add_unknown_data(ComponentId component_id, void *item) -> size_t;
};

/* put the definition here since these are template methods */

template <typename... Args>
auto Table::create_table() -> Table {
  static_assert(sizeof...(Args) > 0, "Can not create table without column");
  static_assert(all_types_are_different<Args...>());

  auto table = Table();
  table.components_ = std::move(Components::from_types<Args...>());
  (table.add_column(ComponentCounter::id<Args>(),
                    Column::create_column<Args>()),
   ...);
  return table;
}

template <typename T>
auto Table::clone_with() const -> Table {
  /* create an empty table with additional column *type* T */

  auto components = components_.template clone_with<T>();
  if (!components.has_value()) {
    throw std::runtime_error(
        fmt::format("Component type `{}` has already existed in table `{}`",
                    typeid(T).name(), table_id_));
  }

  auto table = Table();
  table.components_ = std::move(*components);

  table.add_column(ColumnCounter::id<T>(), Column::create_column<T>());
  for (const auto &[id, column] : columns_) {
    table.add_column(id, column.clone());
  }

  return std::move(table);
}

template <typename T>
auto Table::clone_without() const -> Table {
  /* create an empty table without column *type* T */

  auto components = components_.template clone_without<T>();
  if (!components.has_value()) {
    throw std::runtime_error(
        fmt::format("Component type `{}` does not exist in table `{}`",
                    typeid(T).name(), table_id_));
  }

  if (width_ == 1) {
    throw std::runtime_error("Can not clone to empty table");
  }

  auto table = Table();
  table.components_ = std::move(*components);

  for (const auto &[id, column] : columns_) {
    if (id == ComponentCounter::id<T>()) {
      continue;
    }
    table.add_column(id, column.clone());
  }

  return std::move(table);
}

template <typename T, typename... Args>
auto Table::has_components() const -> bool {
  return components_.has_components<T, Args...>();
}

template <class T>
auto Table::get_column() -> Column & {
  return get_column(ComponentCounter::id<T>());
}

template <class T>
auto Table::get_data(size_t row) -> T & {
  return get_column<T>().template get_data<T>(row);
}

template <typename... Args>
auto Table::get_data_row(size_t row) -> std::tuple<Args &...> {
  return std::tie(get_column<Args>().template get_data<Args>(row)...);
}

template <typename... Args>
void Table::add_data(Args &&...components) {
  static_assert(all_types_are_different<Args...>());

  if (sizeof...(Args) == 0) {
    return;
  }

  all_heights_equal_ = false;
  (get_column<Args>().template push<Args>(std::forward<Args>(components)), ...);
  reset_height();
}

template <typename... Args>
auto Table::add_row(Args &&...components) -> size_t {
  static_assert(all_types_are_different<Args...>());

  if (sizeof...(Args) != width_) {
    throw std::runtime_error(
        fmt::format("Can not add row into table. Row `{}`, width `{}`",
                    sizeof...(components), width_));
  }

  add_data<Args...>(std::forward<Args>(components)...);
  assert(all_heights_equal_ && height_ > 0);

  return height_ - 1;
}

template <typename... Args>
auto Table::begin() -> TableIterator<Args...> {
  return TableIterator<Args...>(height_,
                                get_column<Args>().template begin<Args>()...);
}
template <typename... Args>
auto Table::end() -> TableIterator<Args...> {
  return TableIterator<Args...>(height_,
                                get_column<Args>().template end<Args>()...);
}

#endif
