#ifndef ECS_PRIMITIVE_H
#define ECS_PRIMITIVE_H

#include <atomic>

template <typename T>
class Counter {
 private:
  static std::atomic<int> counter_;

 public:
  template <typename U>
  static auto id() -> int {
    static int value = ++counter_;
    return value;
  }
};

constexpr int INVALID_COUNTER_ID = -1;
template <class T>
std::atomic<int> Counter<T>::counter_{INVALID_COUNTER_ID};

using ComponentId = int;
constexpr ComponentId INVALID_COMPONENT_ID = INVALID_COUNTER_ID;

#endif