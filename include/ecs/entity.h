#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include "ecs/counter.h"
#include "ecs/primitive.h"

class World;

class Entity;
using EntityCounter = InstanceCounter<Entity>;

class Entity {
 public:
  friend class World;

 private:
  explicit Entity(EntityId entity_id);
  EntityId id_;
};

#endif
