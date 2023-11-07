#include "ecs/column.h"

Column::Column(Layout layout) : layout_(layout), capacity_{INITIAL_CAPACITY} {
  reserve(capacity_);
}
Column::~Column() { free(data_); }  // NOLINT
auto Column::size() const -> size_t { return size_; }
auto Column::capacity() const -> size_t { return capacity_; }

void Column::reserve(size_t capacity) {
  auto temp_data = std::realloc(data_, capacity * layout_.size_);  // NOLINT
  if (temp_data == nullptr) {
    throw std::runtime_error("Could not allocate memory");
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