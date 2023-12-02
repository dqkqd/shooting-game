#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include <functional>
#include <type_traits>

#include "ecs/world.h"

template <typename... Args>
class SystemManager {
  enum class SystemType {
    SEQUENTIAL,
    PARALLEL,
  };

  struct System {
    SystemType type;
    std::function<void(World&, std::decay_t<Args>...)> system;
  };

 public:
  template <typename... FuncArgs>
  auto add_sequential(void (*system)(World&, std::decay_t<Args>...,
                                     FuncArgs&...),
                      FuncArgs&... func_args) -> SystemManager& {
    return add(SystemType::SEQUENTIAL, system, func_args...);
  }

  template <typename... FuncArgs>
  auto add_parallel(void (*system)(World&, std::decay_t<Args>..., FuncArgs&...),
                    FuncArgs&... func_args) -> SystemManager& {
    return add(SystemType::PARALLEL, system, func_args...);
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
  std::vector<System> systems_{};

  template <typename... FuncArgs>
  auto add(SystemType type,
           void (*system)(World&, std::decay_t<Args>..., FuncArgs&...),
           FuncArgs&... func_args) -> SystemManager& {
    auto system_without_func_args =
        [&func_args..., system](World& world, std::decay_t<Args>... args) {
          system(world, args..., func_args...);
        };
    systems_.emplace_back(System{type, std::move(system_without_func_args)});
    return *this;
  }
};

#endif
