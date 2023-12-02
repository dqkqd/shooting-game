#ifndef ECS_COLUMN_H
#define ECS_COLUMN_H

#include <spdlog/fmt/fmt.h>

#include <mutex>

#include "ecs/column/iterator.h"
#include "ecs/counter.h"
#include "ecs/primitive.h"

class Column;
using ColumnCounter = Counter<Column>;
using ComponentCounter = ColumnCounter;

class Column {
 public:
  friend class Table;
  explicit Column(int layout = INVALID_LAYOUT,
                  ComponentId component_id = INVALID_COMPONENT_ID);

  Column(const Column &) = delete;
  Column(Column &&column) noexcept;
  auto operator=(const Column &) -> Column & = delete;
  auto operator=(Column &&column) noexcept -> Column &;

  ~Column();

  /* factory */
  template <class T>
  static auto create_column() -> Column;

  // clone a column without copying its content
  [[nodiscard]] auto clone() const -> Column;

  [[nodiscard]] auto component_id() const -> ComponentId;
  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto capacity() const -> size_t;

  template <class T>
  [[nodiscard]] auto is() const -> bool;

  template <class T>
  auto get_data(size_t row) -> std::decay_t<T> &;

  /* data inserting and removing */
  template <class T>
  void push(T &&item);
  template <class T>
  void push_unchecked(T &&item);
  void push_from(Column &other, size_t other_row);
  auto remove(size_t row) -> bool;

  /* iterator helper */
  template <class T>
  auto begin() -> ColumnIterator<T>;
  template <class T>
  auto end() -> ColumnIterator<T>;
  template <class T>
  auto iter() -> ColumnIteratorWrapper<T>;

 private:
  int layout_;
  ComponentId component_id_;

  std::mutex data_mtx_;

  void *data_{};
  size_t size_{};
  size_t capacity_{};

  static constexpr size_t INITIAL_CAPACITY = 4;
  static constexpr int INVALID_LAYOUT = 0;

  [[nodiscard]] auto offset(size_t row) const -> size_t;
  auto get_ptr_at(size_t row) -> void *;

  /* size manipulation */
  void reserve(size_t capacity);
  void grow();
  void shrink();
};

/* put the definition here since these are template methods */

template <class T>
auto Column::create_column() -> Column {
  return Column{sizeof(T), ColumnCounter::id<T>()};
}

template <class T>
auto Column::is() const -> bool {
  return ColumnCounter::id<T>() == component_id_;
}

template <class T>
void Column::push(T &&item) {
  if (!is<T>()) {
    throw std::runtime_error(fmt::format(
        "Can not add data with type `{}` to column", typeid(T).name()));
  }
  push_unchecked<T>(std::forward<T>(item));
}

template <class T>
void Column::push_unchecked(T &&item) {
  grow();
  // placement new
  new (get_ptr_at(size_)) T{std::forward<T>(item)};
  ++size_;
}

template <class T>
auto Column::get_data(size_t row) -> std::decay_t<T> & {
  if (row >= size_) {
    throw std::out_of_range(fmt::format("Could not get data at row {}", row));
  }
  return *std::launder(reinterpret_cast<std::decay_t<T> *>(get_ptr_at(row)));
}

template <class T>
auto Column::begin() -> ColumnIterator<T> {
  std::unique_lock<std::mutex> lock(data_mtx_);
  return ColumnIterator<T>(std::move(lock), static_cast<T *>(data_));
}

template <class T>
auto Column::end() -> ColumnIterator<T> {
  return ColumnIterator<T>(static_cast<T *>(data_) + size_);
}

template <class T>
auto Column::iter() -> ColumnIteratorWrapper<T> {
  return {begin<T>(), end<T>()};
}

#endif
