#ifndef ECS_PRIMITIVE_H
#define ECS_PRIMITIVE_H

#include <atomic>
#include <functional>
#include <optional>
#include <type_traits>

template <typename T>
using Ref = std::reference_wrapper<T>;

template <typename T>
using OptionalRef = std::optional<Ref<T>>;

constexpr int INVALID_COUNTER_ID = -1;

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

template <typename T, typename... Ts>
constexpr bool all_types_are_same =  // NOLINT
    std::conjunction_v<std::is_same<T, Ts>...>;

// pairwise different type compile time check
template <typename T = int>
constexpr auto all_types_are_different() -> bool {
  return true;
}
template <typename T, typename U, typename... Args>
constexpr auto all_types_are_different() -> bool {
  return !std::is_same_v<T, U> && all_types_are_different<T, Args...>() &&
         all_types_are_different<U, Args...>();
}

// custom hash function based on boost::hash_combine
template <class T>
inline void hash_combine(std::size_t& seed, const T& value) {
  std::hash<T> hasher{};
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif
