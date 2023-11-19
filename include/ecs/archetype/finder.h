#ifndef ECS_ARCHETYPE_FINDER_H
#define ECS_ARCHETYPE_FINDER_H

#include "ecs/column.h"
#include "ecs/primitive.h"

class ArchetypesFinder {
 public:
  ArchetypesFinder() = default;

  void add(ComponentId component_id, ArchetypeId archetype_id) {
    by_component_[component_id].emplace_back(archetype_id);
  }

  template <typename... Args>
  auto get() const -> std::vector<ArchetypeId> {
    std::unordered_map<ArchetypeId, int> archetypes_occurrences;
    (
        [&] {
          for (const auto &archetype_id :
               by_component_.at(ComponentCounter::id<Args>())) {
            ++archetypes_occurrences[archetype_id];
          }
        }(),
        ...);
    std::vector<ArchetypeId> results;
    for (const auto &[archetype_id, count] : archetypes_occurrences) {
      if (count == sizeof...(Args)) {
        results.emplace_back(archetype_id);
      }
    }
    std::reverse(results.begin(), results.end());
    return results;
  }

 private:
  std::unordered_map<ComponentId, std::vector<ArchetypeId>> by_component_;
};

#endif
