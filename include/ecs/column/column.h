#ifndef ECS_COLUMN_H
#define ECS_COLUMN_H

#include <spdlog/fmt/fmt.h>

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <typeinfo>

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

  template <class T>
  static auto create_column() -> Column {
    return Column{sizeof(T), ColumnCounter::id<T>()};
  }

  // clone a column without copying its content
  [[nodiscard]] auto clone() const -> Column;

  template <class T>
  auto is() -> bool {
    return ColumnCounter::id<T>() == component_id_;
  }

  template <class T>
  void push(T &&item) {
    if (!is<T>()) {
      throw std::runtime_error(fmt::format(
          "Can not add data with type `{}` to column", typeid(T).name()));
    }
    push_unchecked<T>(std::forward<T>(item));
  }

  template <class T>
  void push_unchecked(T &&item) {
    grow();
    // placement new
    new (get_ptr_at(size_)) T{std::forward<T>(item)};
    ++size_;
  }

  void push_unknown(void *item);

  auto remove(size_t row) -> bool;

  [[nodiscard]] auto component_id() const -> ComponentId;
  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto capacity() const -> size_t;

  template <class T>
  auto get_data(size_t row) -> std::decay_t<T> & {
    if (row >= size_) {
      throw std::out_of_range(fmt::format("Could not get data at row {}", row));
    }
    return *std::launder(reinterpret_cast<std::decay_t<T> *>(get_ptr_at(row)));
  }

  template <class T>
  auto iter() -> ColumnIteratorWrapper<T> {
    return {begin<T>(), end<T>()};
  }

  template <class T>
  auto begin() -> ColumnIterator<T> {
    return ColumnIterator<T>(static_cast<T *>(data_));
  }

  template <class T>
  auto end() -> ColumnIterator<T> {
    return ColumnIterator<T>(static_cast<T *>(data_) + size_);
  }

 private:
  [[nodiscard]] auto offset(size_t row) const -> size_t;
  auto get_ptr_at(size_t row) -> void *;

  void reserve(size_t capacity);
  void grow();
  void shrink();

  int layout_;
  ComponentId component_id_;

  void *data_{};
  size_t size_{};
  size_t capacity_{};

  static constexpr size_t INITIAL_CAPACITY = 4;
  static constexpr int INVALID_LAYOUT = 0;
};

#endif
