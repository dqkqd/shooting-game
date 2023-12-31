#include "ecs/column/column.h"

#include <utility>

Column::Column(Column &&column) noexcept
    : layout_{std::exchange(column.layout_, INVALID_LAYOUT)},
      component_id_{std::exchange(column.component_id_, INVALID_COMPONENT_ID)},
      data_{std::exchange(column.data_, nullptr)},
      size_{std::exchange(column.size_, 0)},
      capacity_{std ::exchange(column.capacity_, 0)} {}

auto Column::operator=(Column &&column) noexcept -> Column & {
  layout_ = std::exchange(column.layout_, {});
  component_id_ = std::exchange(column.component_id_, INVALID_COMPONENT_ID);
  data_ = std::exchange(column.data_, nullptr);
  size_ = std::exchange(column.size_, 0);
  capacity_ = std ::exchange(column.capacity_, 0);
  return *this;
};

Column::Column(int layout, ComponentId component_id)  // NOLINT
    : layout_(layout),
      component_id_{component_id},
      capacity_{INITIAL_CAPACITY} {
  reserve(capacity_);
}
Column::~Column() {
  std::destroy_n(reinterpret_cast<std::uintptr_t *>(data_), offset(capacity_));
  free(data_);  // NOLINT
}

auto Column::clone() const -> Column { return Column(layout_, component_id_); }

void Column::push_from(Column &other, size_t other_row) {
  grow();
  std::memcpy(get_ptr_at(size_), other.get_ptr_at(other_row), layout_);
  ++size_;
}

auto Column::remove(size_t row) -> bool {
  if (row >= size_) {
    return false;
  }
  std::memcpy(get_ptr_at(row), get_ptr_at(size_ - 1), layout_);
  --size_;
  shrink();
  return true;
}

auto Column::component_id() const -> ComponentId { return component_id_; }
auto Column::is_valid() const -> bool {
  return layout_ != INVALID_LAYOUT && component_id_ != INVALID_COMPONENT_ID;
}
auto Column::size() const -> size_t { return size_; }
auto Column::capacity() const -> size_t { return capacity_; }

auto Column::offset(size_t row) const -> size_t { return row * layout_; }
auto Column::get_ptr_at(size_t row) -> void * {
  auto ptr = reinterpret_cast<std::uintptr_t>(data_);
  return reinterpret_cast<void *>(ptr + offset(row));  // NOLINT
}

void Column::reserve(size_t capacity) {
  auto temp_data = std::realloc(data_, capacity * layout_);  // NOLINT
  if (temp_data == nullptr) {
    throw std::runtime_error("Can not allocate memory");
  }
  data_ = temp_data;
  capacity_ = capacity;
}

void Column::grow() {
  if (size_ >= capacity_) {
    reserve(capacity_ * 2);
  }
}

void Column::shrink() {
  if (size_ < capacity_ / 4) {
    reserve(capacity_ / 2);
  }
}
