#ifndef ECS_PRIMITIVE_H
#define ECS_PRIMITIVE_H

using ComponentId = int;

class ComponentIdCounter {
 private:
  static int counter_;  // NOLINT
 public:
  template <class T>
  static auto get() -> ComponentId {
    static ComponentId component_id = ++counter_;
    return component_id;
  }
};

#endif