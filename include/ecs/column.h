#ifndef ECS_COLUMN_H
#define ECS_COLUMN_H

#include <format>
#include <typeinfo>

class Column {
 public:
  Column(const Column &) = delete;
  Column(Column &&) = delete;
  auto operator=(const Column &) -> Column & = delete;
  auto operator=(Column &&) -> Column & = delete;

  ~Column();

  template <class T>
  static auto create_column() -> Column {
    return Column{Layout{typeid(T).hash_code(), sizeof(T)}};
  }

  template <class T>
  auto is() -> bool {
    return typeid(T).hash_code() == layout_.id_;
  }

  template <class T>
  void push(T item) {
    if (!is<T>()) {
      throw std::runtime_error(
          std::format("Could not add type {} into column", typeid(T).name())
              .c_str());
    }
    grow();
    get_data_unchecked<T>(size_) = std::move(item);
    ++size_;
  }

  [[nodiscard]] auto size() const -> size_t;
  [[nodiscard]] auto capacity() const -> size_t;

  template <class T>
  auto get_data_unchecked(size_t row) -> T & {
    auto ptr = reinterpret_cast<T *>(data_);
    return ptr[row];
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
  struct Layout {
    std::size_t id_;
    int size_;
  };

  explicit Column(Layout layout);

  void reserve(size_t capacity);
  void grow();
  void shrink();

  Layout layout_;
  void *data_{};
  size_t size_{};
  size_t capacity_{};
};

#endif
