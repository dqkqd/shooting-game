#ifndef ECS_PRIMITIVE_H
#define ECS_PRIMITIVE_H

#include <atomic>
#include <type_traits>

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

#endif
