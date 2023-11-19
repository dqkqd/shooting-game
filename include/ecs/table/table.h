#ifndef ECS_TABLE_TABLE_H
#define ECS_TABLE_TABLE_H

#include "ecs/column.h"
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

  template <typename... Args>
  static auto create_table() -> Table {
    static_assert(sizeof...(Args) > 0, "Can not create table without column");
    static_assert(all_types_are_different<Args...>(),
                  "All column types must be pairwise different");

    auto table = Table();
    table.components_ = std::move(Components::from_types<Args...>());
    table.width_ = sizeof...(Args);
    (
        [&] {
          table.columns_[ComponentCounter::id<Args>()] =
              Column::create_column<Args>();
        }(),
        ...);
    return table;
  }

  template <typename T>
  auto clone_with() const -> Table {
    /* create an empty table with additional column *type* T */

    if (has_components<T>()) {
      throw std::runtime_error(
          fmt::format("Component type `{}` has already existed in table `{}`",
                      typeid(T).name(), table_id_));
    }

    auto table = create_table<T>();
    table.components_ = std::move(components_.template clone_with<T>().value());

    for (const auto &[id, column] : columns_) {
      table.columns_.emplace(id, column.clone());
      ++table.width_;
    }
    return std::move(table);
  }

  template <typename T>
  auto clone_without() const -> Table {
    /* create an empty table without column *type* T */

    if (!has_components<T>()) {
      throw std::runtime_error(
          fmt::format("Component type `{}` does not exist in table `{}`",
                      typeid(T).name(), table_id_));
    }

    if (width_ == 1) {
      throw std::runtime_error("Can not clone to empty table");
    }

    auto table = Table();
    table.components_ =
        std::move(components_.template clone_without<T>().value());

    for (const auto &[id, column] : columns_) {
      if (id == ComponentCounter::id<T>()) {
        continue;
      }
      table.columns_.emplace(id, column.clone());
      ++table.width_;
    }
    return std::move(table);
  }

  [[nodiscard]] auto table_id() const -> TableId;
  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto is_empty() const -> bool;
  [[nodiscard]] auto width() const -> size_t;
  [[nodiscard]] auto height() const -> size_t;

  [[nodiscard]] auto components() const -> const Components &;

  [[nodiscard]] auto has_component_id(ComponentId component_id) const -> bool;

  template <typename... Args, typename = std::enable_if_t<
                                  all_types_are_same<ComponentId, Args...>>>
  auto has_components(ComponentId component_id, Args... component_ids) const
      -> bool {
    return has_component_id(component_id) &&
           (... && has_component_id(component_ids));
  }
  template <typename T, typename... Args>
  auto has_components() const -> bool {
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
    return get_column<T>().template data_at<T>(row);
  }

  // add component to a column without increase height
  auto add_component(ComponentId component_id, void *item) -> size_t;
  template <typename T>
  auto add_component(T &&item) -> size_t {
    all_heights_equal_ = false;
    Column &column = get_column<T>();
    column.push<T>(std::forward<T>(item));
    return column.size();
  }

  template <typename... Args>
  void add_components(Args &&...components) {
    static_assert(all_types_are_different<Args...>(),
                  "All column types must be pairwise different");
    ([&] { add_component<Args>(std::forward<Args>(components)); }(), ...);
  }

  template <typename... Args>
  auto add_row(Args &&...components) -> size_t {
    if (sizeof...(components) != width_) {
      throw std::runtime_error(
          fmt::format("Can not add row into table. Row `{}`, width `{}`",
                      sizeof...(components), width_));
    }

    add_components<Args...>(std::forward<Args>(components)...);

    // since all types are different and number of types equal width_, we can
    // certain that if all the component arguments can be added to the table,
    // they must be all the components we have, hence we can aggressively set
    // this to true without checking
    all_heights_equal_ = true;

    ++height_;
    size_t row_index = height_ - 1;
    return row_index;
  }

  auto remove_row(size_t row) -> bool;
  auto move_row_to_other(size_t row, Table &other) -> std::optional<size_t>;
  void reset_height();

  template <typename... Args>
  auto begin() -> TableIterator<Args...> {
    return TableIterator<Args...>(height_,
                                  get_column<Args>().template begin<Args>()...);
  }
  template <typename... Args>
  auto end() -> TableIterator<Args...> {
    return TableIterator<Args...>(height_,
                                  get_column<Args>().template end<Args>()...);
  }

 private:
  TableId table_id_;
  Components components_{};
  std::unordered_map<ComponentId, Column> columns_{};

  size_t width_;
  size_t height_;

  bool all_heights_equal_ = true;
};

#endif
