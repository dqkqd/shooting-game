#ifndef ECS_COLUMN_ITERATOR_H
#define ECS_COLUMN_ITERATOR_H

#include <iterator>

template <class T>
class ColumnIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T *;
  using reference = T &;

  explicit ColumnIterator(pointer ptr = nullptr) : ptr_{ptr} {}

  auto operator*() const -> reference { return *ptr_; }
  auto operator->() -> pointer { return ptr_; }
  auto operator++() -> ColumnIterator & {
    ptr_++;
    return *this;
  }

  friend auto operator==(const ColumnIterator &lhs, const ColumnIterator &rhs)
      -> bool {
    return lhs.ptr_ == rhs.ptr_;
  };
  friend auto operator!=(const ColumnIterator &lhs, const ColumnIterator &rhs)
      -> bool {
    return lhs.ptr_ != rhs.ptr_;
  };

 private:
  pointer ptr_;
};

template <class T>
class ColumnIteratorWrapper {
 public:
  ColumnIteratorWrapper(ColumnIterator<T> begin, ColumnIterator<T> end)
      : begin_{begin}, end_{end} {}
  auto begin() -> ColumnIterator<T> { return begin_; }
  auto end() -> ColumnIterator<T> { return end_; }

 private:
  ColumnIterator<T> begin_;
  ColumnIterator<T> end_;
};

#endif
