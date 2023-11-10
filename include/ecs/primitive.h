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

using ComponentId = int;
constexpr ComponentId INVALID_COMPONENT_ID = INVALID_COUNTER_ID;

using ColumnId = ComponentId;
constexpr ColumnId INVALID_COLUMN_ID = INVALID_COMPONENT_ID;

using TableId = int;
constexpr TableId INVALID_TABLE_ID = INVALID_COUNTER_ID;

using ArchetypeId = int;
constexpr ArchetypeId INVALID_ARCHETYPE_ID = INVALID_COUNTER_ID;

using EntityId = int;
constexpr EntityId INVALID_ENTITY_ID = INVALID_COUNTER_ID;

#endif