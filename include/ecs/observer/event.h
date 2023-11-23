#ifndef ECS_OBSERVER_EVENT_H
#define ECS_OBSERVER_EVENT_H

#include "ecs/component.h"
#include "ecs/primitive.h"

enum class EventType { ADD_ARCHETYPE };

class Event {  // NOLINT
 public:
  explicit Event(EventType type) : type_{type} {}
  virtual ~Event() = default;

  [[nodiscard]] auto type() const -> EventType { return type_; }

  template <typename T>
  auto as() -> T* {
    return dynamic_cast<T*>(this);
  }

 private:
  EventType type_;
};

class AddArchetypeEvent : public Event {
 public:
  AddArchetypeEvent(Components&& components, ArchetypeId archetype_id)
      : Event(EventType::ADD_ARCHETYPE),
        components_{std::move(components)},
        archetype_id_{archetype_id} {}

  [[nodiscard]] auto components() -> Components& { return components_; }
  [[nodiscard]] auto archetype_id() const -> ArchetypeId {
    return archetype_id_;
  }

 private:
  Components components_;
  ArchetypeId archetype_id_;
};

#endif
