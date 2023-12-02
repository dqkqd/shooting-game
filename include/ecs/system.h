#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include <functional>
#include <thread>

enum class SystemType {
  SEQUENTIAL,
  PARALLEL,
};

struct System {
  SystemType type;
  std::function<void()> system;
};

class SystemManager {
 public:
  void add(SystemType type, std::function<void()>&& system) {
    systems_.emplace_back(System{type, std::move(system)});
  }

  void run() {
    std::size_t index = 0;
    while (index < systems_.size()) {
      if (systems_[index].type == SystemType::SEQUENTIAL) {
        systems_[index].system();
        ++index;
      } else {
        // collect all parallel systems
        std::vector<std::thread> threads;
        while (index < systems_.size() &&
               systems_[index].type == SystemType::PARALLEL) {
          threads.emplace_back([this, index]() { systems_[index].system(); });
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
};

#endif
