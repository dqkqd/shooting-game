#ifndef ECS_PRIMITIVE_H
#define ECS_PRIMITIVE_H

#include <atomic>

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

#endif