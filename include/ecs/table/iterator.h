#ifndef ECS_TABLE_ITERATOR_H
#define ECS_TABLE_ITERATOR_H

#include <iterator>

#include "ecs/column/iterator.h"

template <typename... Args>
class TableIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::tuple<Args...>;
  using pointer = std::tuple<Args *...>;
  using reference = std::tuple<Args &...>;

  TableIterator() = default;
  explicit TableIterator(std::size_t current_row,  // NOLINT
                         std::size_t max_rows,
                         ColumnIterator<Args> &&...column_iters)
      : current_row_{current_row},
        max_rows_{max_rows},
        iters_(std::make_tuple(std::move(column_iters)...)) {}

  TableIterator(const TableIterator &) = delete;
  auto operator=(const TableIterator &) -> TableIterator & = delete;

  TableIterator(TableIterator &&table_iter) noexcept
      : current_row_{table_iter.current_row_},
        max_rows_{table_iter.max_rows_},
        iters_{std::move(table_iter.iters_)} {};
  auto operator=(TableIterator &&table_iter) noexcept -> TableIterator & {
    current_row_ = table_iter.current_row_;
    max_rows_ = table_iter.max_rows_;
    iters_ = std::move(table_iter.iters_);
    return *this;
  };

  ~TableIterator() = default;

  [[nodiscard]] auto done() const -> bool { return current_row_ == max_rows_; }

  auto operator*() -> reference { return dereference(); }
  auto operator++() -> TableIterator & {
    advance();
    return *this;
  }

  friend auto operator==(const TableIterator &lhs, const TableIterator &rhs)
      -> bool {
    return lhs.equal(rhs);
  };
  friend auto operator!=(const TableIterator &lhs, const TableIterator &rhs)
      -> bool {
    return lhs.not_equal(rhs);
  };

 private:
  size_t current_row_{};
  size_t max_rows_{};
  std::tuple<ColumnIterator<Args>...> iters_;

  auto dereference() -> reference {
    return dereference_impl(std::index_sequence_for<Args...>());
  }
  template <size_t... Is>
  auto dereference_impl(std::index_sequence<Is...> /**/) -> reference {
    return std::tie(*std::get<Is>(iters_)...);
  }

  void advance() { return advance_impl(std::index_sequence_for<Args...>()); }
  template <size_t... Is>
  auto advance_impl(std::index_sequence<Is...> /**/) {
    ++current_row_;
    (++std::get<Is>(iters_), ...);
  }

  auto equal(const TableIterator &other) const -> bool {
    return equal_impl(other, std::index_sequence_for<Args...>());
  }
  template <size_t... Is>
  auto equal_impl(const TableIterator &other,
                  std::index_sequence<Is...> /**/) const -> bool {
    return (... && (std::get<Is>(iters_) == std::get<Is>(other.iters_)));
  }

  auto not_equal(const TableIterator &other) const -> bool {
    return not_equal_impl(other, std::index_sequence_for<Args...>());
  }
  template <size_t... Is>
  auto not_equal_impl(const TableIterator &other,
                      std::index_sequence<Is...> /**/) const -> bool {
    return (... || (std::get<Is>(iters_) != std::get<Is>(other.iters_)));
  }
};

template <class... Args>
class TableIteratorWrapper {
 public:
  TableIteratorWrapper(TableIterator<Args...> &&begin,
                       TableIterator<Args...> &&end)
      : begin_{std::move(begin)}, end_{std::move(end)} {}

  TableIteratorWrapper(const TableIteratorWrapper &) = delete;
  auto operator=(const TableIteratorWrapper &)
      -> TableIteratorWrapper & = delete;

  TableIteratorWrapper(TableIteratorWrapper &&table_iterator_wrapper) noexcept
      : begin_{std::move(table_iterator_wrapper.begin_)},
        end_{std::move(table_iterator_wrapper.end_)} {};
  auto operator=(TableIteratorWrapper &&table_iterator_wrapper) noexcept
      -> TableIteratorWrapper & {
    begin_ = std::move(table_iterator_wrapper.begin_);
    end_ = std::move(table_iterator_wrapper.end_);
    return *this;
  };

  ~TableIteratorWrapper() = default;
  auto begin() -> TableIterator<Args...> { return std::move(begin_); }
  auto end() -> TableIterator<Args...> { return std::move(end_); }

 private:
  TableIterator<Args...> begin_;
  TableIterator<Args...> end_;
};

#endif
