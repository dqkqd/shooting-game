
#include "ecs/observer/event.h"
#include "ecs/observer/observer.h"
#include "gtest/gtest.h"

class EventTest : public Event {
 public:
  EventTest() : Event(EventType::ADD_ARCHETYPE) {}
};

class ListenerTest : public EventListener {
 public:
  int id;
  int value{};
  explicit ListenerTest(int listener_id) : id{listener_id} {};
  void on_notify(Event* e [[maybe_unused]]) override { value += 1; }
};

class ObserverTest {
 public:
  EventManager manager;
  void notify() {
    auto event = EventTest();
    manager.notify(&event);
  }
};

TEST(Observer, AddAndRemove) {
  auto observer = ObserverTest();
  auto listener1 = ListenerTest(1);
  auto listener2 = ListenerTest(2);

  observer.manager.add_listener(listener1);
  observer.manager.add_listener(listener2);

  observer.notify();
  EXPECT_EQ(listener1.value, 1);
  EXPECT_EQ(listener2.value, 1);

  observer.manager.remove_listener(listener1);
  observer.notify();
  EXPECT_EQ(listener1.value, 1);
  EXPECT_EQ(listener2.value, 2);
};
