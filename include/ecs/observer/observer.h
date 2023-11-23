#ifndef ECS_OBSERVER_OBSERVER_H
#define ECS_OBSERVER_OBSERVER_H

#include <functional>
#include <list>
#include <memory>

#include "ecs/observer/event.h"

class EventListener {  // NOLINT
 public:
  virtual ~EventListener() = default;
  virtual void on_notify(Event*) = 0;
};

class EventManager {
 public:
  void add_listener(EventListener& listener) {
    listeners_.push_back(std::ref(listener));
  }
  void remove_listener(EventListener& listener) {
    listeners_.remove_if([&](auto other) {
      return std::addressof(other.get()) == std::addressof(listener);
    });
  }

  void notify(Event* event) {
    for (auto listener : listeners_) {
      listener.get().on_notify(event);
    }
  }

 private:
  std::list<std::reference_wrapper<EventListener>> listeners_;
};
#endif
