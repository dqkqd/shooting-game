#ifndef ECS_COUNTER_H
#define ECS_COUNTER_H

#include <atomic>

#include "ecs/primitive.h"

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

template <class T>
std::atomic<int> Counter<T>::counter_{INVALID_COUNTER_ID};

template <class T>
class InstanceCounter {
 private:
  static std::atomic<int> counter_;

 public:
  static auto id() -> int;
};

template <class T>
std::atomic<int> InstanceCounter<T>::counter_ = INVALID_COUNTER_ID;
template <class T>
auto InstanceCounter<T>::id() -> int {
  return ++counter_;
}

#endif