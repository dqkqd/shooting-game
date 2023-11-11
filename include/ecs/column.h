#ifndef ECS_COLUMN_H
#define ECS_COLUMN_H

#include <cstddef>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include "ecs/counter.h"
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
      std::ostringstream error_msg;
      error_msg << "Could not add type " << typeid(T).name() << " to column"
                << std::endl;
      throw std::runtime_error(error_msg.str());
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

  [[nodiscard]] auto component_id() const -> ComponentId;
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

  template <class T>
  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    explicit Iterator(pointer ptr) : ptr_{ptr} {}

    auto operator*() const -> reference { return *ptr_; }
    auto operator->() -> pointer { return ptr_; }
    auto operator++() -> Iterator & {
      ptr_++;
      return *this;
    }
    auto operator++(int) -> Iterator {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    friend auto operator==(const Iterator &lhs, const Iterator &rhs) -> bool {
      return lhs.ptr_ == rhs.ptr_;
    };
    friend auto operator!=(const Iterator &lhs, const Iterator &rhs) -> bool {
      return lhs.ptr_ != rhs.ptr_;
    };

   private:
    pointer ptr_;
  };

  template <class T>
  class IteratorWrapper {
   public:
    IteratorWrapper(Iterator<T> begin, Iterator<T> end)
        : begin_{begin}, end_{end} {}
    auto begin() -> Iterator<T> { return begin_; }
    auto end() -> Iterator<T> { return end_; }

   private:
    Iterator<T> begin_;
    Iterator<T> end_;
  };

  template <class T>
  auto iter() -> IteratorWrapper<T> {
    return {begin<T>(), end<T>()};
  }

  template <class T>
  auto begin() -> Iterator<T> {
    return Iterator<T>(static_cast<T *>(data_));
  }

  template <class T>
  auto end() -> Iterator<T> {
    return Iterator<T>(static_cast<T *>(data_) + size_);
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
