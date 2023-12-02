#ifndef ECS_SYSTEMS_CORE_H
#define ECS_SYSTEMS_CORE_H

#include <functional>
#include <type_traits>

#include "ecs/world.h"

namespace sys {
enum class SystemType {
  SEQUENTIAL,
  PARALLEL,
};

template <typename... Args>
struct System {
  SystemType type;
  std::function<void(World&, std::decay_t<Args>...)> system;
};

template <typename... Args>
class SystemManager {
 public:
  template <typename... FuncArgs>
  void add(SystemType type,
           const std::function<void(World&, std::decay_t<Args>...,
                                    std::decay_t<FuncArgs>...)>& system,
           FuncArgs&... func_args) {
    auto system_without_func_args =
        [&func_args..., system](World& world, std::decay_t<Args>... args) {
          system(world, args..., func_args...);
        };
    systems_.emplace_back(
        System<Args...>{type, std::move(system_without_func_args)});
  }

  void run(World& world, Args&... args) {
    std::size_t index = 0;
    while (index < systems_.size()) {
      if (systems_[index].type == SystemType::SEQUENTIAL) {
        systems_[index].system(world, args...);
        ++index;
      } else {
        // collect all parallel systems
        std::vector<std::thread> threads;
        while (index < systems_.size() &&
               systems_[index].type == SystemType::PARALLEL) {
          threads.emplace_back([this, &world, index, &args...]() {
            systems_[index].system(world, args...);
          });
          ++index;
        }

        // run all parallel systems
        for (auto& t : threads) {
          t.join();
        }
      }
    }
  }

 private:
  std::vector<System<Args...>> systems_{};
};

};  // namespace sys
#endif
