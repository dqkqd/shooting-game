#include <gtest/gtest.h>

#include "ecs/archetype/finder.h"

TEST(ArchetypesFinder, AddAndGet) {
  auto archetypes_finder = ArchetypesFinder();
  archetypes_finder.add(ComponentCounter::id<int>(), 1);
  archetypes_finder.add(ComponentCounter::id<int>(), 2);
  archetypes_finder.add(ComponentCounter::id<int>(), 3);
  archetypes_finder.add(ComponentCounter::id<float>(), 2);
  archetypes_finder.add(ComponentCounter::id<float>(), 3);
  archetypes_finder.add(ComponentCounter::id<float>(), 4);

  auto int_lookup_result = archetypes_finder.get<int>();
  EXPECT_EQ(int_lookup_result, (std::vector{1, 2, 3}));

  auto float_lookup_result = archetypes_finder.get<float>();
  EXPECT_EQ(float_lookup_result, (std::vector{2, 3, 4}));

  auto int_and_float_lookup_result = archetypes_finder.get<int, float>();
  EXPECT_EQ(int_and_float_lookup_result, (std::vector{2, 3}));
}

TEST(ArchetypesFinder, FoundNothing) {
  auto archetypes_finder = ArchetypesFinder();
  archetypes_finder.add(ComponentCounter::id<int>(), 1);
  auto result = archetypes_finder.get<float>();
  EXPECT_TRUE(result.empty());
}
