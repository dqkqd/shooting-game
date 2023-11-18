#include <gtest/gtest.h>

#include "ecs/archetype/archetype_components.h"
#include "ecs/column.h"

TEST(ArchetypeComponents, MoveConstructor) {
  auto components = ArchetypeComponents::from_types<int, float, std::string>();
  auto components2 = std::move(components);
  EXPECT_EQ(components2.size(), 3);
  EXPECT_EQ(components.size(), 0);
}

TEST(ArchetypeComponents, Comparison) {
  EXPECT_EQ((ArchetypeComponents::from_types<int, float>()),
            ((ArchetypeComponents::from_types<float, int>())));
  EXPECT_LT((ArchetypeComponents::from_types<int, float>()),
            ((ArchetypeComponents::from_types<float, int, double>())));
}

TEST(ArchetypeComponents, VecAndSetConstruct) {
  EXPECT_EQ((ArchetypeComponents::from_types<int, float>()),
            ArchetypeComponents::from_vec(
                {ComponentCounter::id<int>(), ComponentCounter::id<float>()}));
  EXPECT_EQ((ArchetypeComponents::from_types<int, float>()),
            ArchetypeComponents::from_set(
                {ComponentCounter::id<int>(), ComponentCounter::id<float>()}));
}

TEST(ArchetypeComponents, Clone) {
  auto components = ArchetypeComponents::from_types<int, float, char>();
  EXPECT_EQ(components.clone(),
            (ArchetypeComponents::from_types<int, float, char>()));
  EXPECT_EQ(components.clone(), components);
}

TEST(ArchetypeComponents, CloneWith) {
  auto components = ArchetypeComponents::from_types<int, float, char>();
  auto cloned = components.clone_with<std::string>();
  EXPECT_EQ(cloned,
            (ArchetypeComponents::from_types<int, float, char, std::string>()));
  // existed component
  EXPECT_FALSE((components.clone_with<float>()).has_value());
}

TEST(ArchetypeComponents, CloneWithout) {
  auto components = ArchetypeComponents::from_types<int, float, char>();
  auto cloned = components.clone_without<int, char>();
  EXPECT_EQ(cloned, (ArchetypeComponents::from_types<float>()));
  // non existed component
  EXPECT_FALSE((components.clone_without<std::string>()).has_value());
}
