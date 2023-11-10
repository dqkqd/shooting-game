#ifndef ECS_COLUMN_H
#define ECS_COLUMN_H

#include <format>
#include <typeinfo>

#include "ecs/primitive.h"

class Column;
using ColumnCounter = Counter<Column>;
using ComponentCounter = ColumnCounter;

class Column {
 public:
  explicit Column(int layout = INVALID_LAYOUT,
                  ComponentId component_id = INVALID_COMPONENT_ID);

  Column(const Column &) = delete;
  Column(Column &&column) noexcept;
  auto operator=(const Column &) -> Column & = delete;
  auto operator=(Column &&column) noexcept -> Column &;

  ~Column();

  [[nodiscard]] auto component_id() const -> ComponentId;

  template <class T>
  static auto create_column() -> Column {
    return Column{sizeof(T), ColumnCounter::id<T>()};
  }

  template <class T>
  auto is() -> bool {
    return ColumnCounter::id<T>() == component_id_;
  }

  template <class T>
  void push(T &&item) {
    if (!is<T>()) {
      throw std::runtime_error(
          std::format("Could not add type {} into column", typeid(T).name())
              .c_str());
    }
    push_unchecked<T>(std::forward<T>(item));
  }

  template <class T>
  void push_unchecked(T item) {
    grow();
    // placement new
    new (get_ptr_at(size_)) T{std::move(item)};
    ++size_;
  }

  [[nodiscard]] auto is_valid() const -> bool;
  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto capacity() const -> size_t;

  template <class T>
  auto get_data_unchecked(size_t row) -> std::decay_t<T> & {
    return *std::launder(reinterpret_cast<std::decay_t<T> *>(get_ptr_at(row)));
  }

  template <class T>
  auto get_data(size_t row) -> std::optional<std::reference_wrapper<T>> {
    if (row >= size_) {
      return {};
    }
    std::reference_wrapper<T> data = get_data_unchecked<T>(row);
    return std::move(data);
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
