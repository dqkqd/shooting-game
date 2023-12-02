#ifndef ECS_COLUMN_ITERATOR_H
#define ECS_COLUMN_ITERATOR_H

#include <iterator>
#include <mutex>
#include <utility>

template <class T>
class ColumnIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T *;
  using reference = T &;

  explicit ColumnIterator(pointer ptr = nullptr) : ptr_{ptr} {}
  explicit ColumnIterator(std::unique_lock<std::mutex> &&lock,
                          pointer ptr = nullptr)
      : lock_{std::move(lock)}, ptr_{ptr} {}

  ColumnIterator(const ColumnIterator &) = delete;
  auto operator=(const ColumnIterator &) -> ColumnIterator & = delete;
  ColumnIterator(ColumnIterator &&column_iter) noexcept
      : lock_{std::move(column_iter.lock_)},
        ptr_{std::exchange(column_iter.ptr_, nullptr)} {};
  auto operator=(ColumnIterator &&column_iter) noexcept -> ColumnIterator & {
    lock_ = std::move(column_iter.lock_);
    ptr_ = std::exchange(column_iter.ptr_, nullptr);
    return *this;
  };
  ~ColumnIterator() = default;

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
  std::unique_lock<std::mutex> lock_;
  pointer ptr_;
};

template <class T>
class ColumnIteratorWrapper {
 public:
  ColumnIteratorWrapper(ColumnIterator<T> &&begin, ColumnIterator<T> &&end)
      : begin_{std::move(begin)}, end_{std::move(end)} {}

  ColumnIteratorWrapper(const ColumnIteratorWrapper &) = delete;
  ColumnIteratorWrapper &operator=(const ColumnIteratorWrapper &) = delete;

  ColumnIteratorWrapper(ColumnIteratorWrapper &&column_iter_wrapper) noexcept
      : begin_{std::move(column_iter_wrapper.begin_)},
        end_{std::move(column_iter_wrapper.end_)} {};
  auto operator=(ColumnIteratorWrapper &&column_iter_wrapper) noexcept
      -> ColumnIteratorWrapper & {
    begin_ = std::move(column_iter_wrapper.begin_);
    end_ = std::move(column_iter_wrapper.end_);
    return *this;
  };

  ~ColumnIteratorWrapper() = default;

  auto begin() -> ColumnIterator<T> { return std::move(begin_); }
  auto end() -> ColumnIterator<T> { return std::move(end_); }

 private:
  ColumnIterator<T> begin_;
  ColumnIterator<T> end_;
};

#endif
