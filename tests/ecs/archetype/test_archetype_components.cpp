#include <gtest/gtest.h>

#include "ecs/archetype/archetype_components.h"
#include "ecs/column.h"

TEST(ArchetypeComponents, HasComponent) {
  auto id1 = ComponentCounter::id<int>();
  auto id2 = ComponentCounter::id<float>();
  auto components =
      ArchetypeComponents::create_archetype_components<int, float>();

  EXPECT_TRUE(components.has_components(id1, id2));
  EXPECT_FALSE(
      components.has_components(id1, id2, ComponentCounter::id<double>()));

  EXPECT_TRUE((components.has_components<int, float>()));
  EXPECT_FALSE((components.has_components<int, float, double>()));
}

TEST(ArchetypeComponents, MoveConstructor) {
  auto components =
      ArchetypeComponents::create_archetype_components<int, float,
                                                       std::string>();
  auto components2 = std::move(components);
  EXPECT_TRUE(components2.has_components<int>());
  EXPECT_FALSE(components.has_components<int>());
}

TEST(ArchetypeComponents, Comparison) {
  EXPECT_EQ((ArchetypeComponents::create_archetype_components<int, float>()),
            ((ArchetypeComponents::create_archetype_components<float, int>())));
  EXPECT_LT((ArchetypeComponents::create_archetype_components<int, float>()),
            ((ArchetypeComponents::create_archetype_components<float, int,
                                                               double>())));
}

TEST(ArchetypeComponents, CloneWith) {
  auto components =
      ArchetypeComponents::create_archetype_components<int, float, char>();
  auto cloned = components.clone_with<std::string>();
  EXPECT_EQ(cloned,
            (ArchetypeComponents::create_archetype_components<int, float, char,
                                                              std::string>()));
  // duplicated components
  EXPECT_FALSE((components.clone_with<int, int>()).has_value());
  // existed component
  EXPECT_FALSE((components.clone_with<float>()).has_value());
}

TEST(ArchetypeComponents, CloneWithout) {
  auto components =
      ArchetypeComponents::create_archetype_components<int, float, char>();
  auto cloned = components.clone_without<int, char>();
  EXPECT_EQ(cloned,
            (ArchetypeComponents::create_archetype_components<float>()));
  // duplicated components
  EXPECT_FALSE((components.clone_without<int, int>()).has_value());
  // non existed component
  EXPECT_FALSE((components.clone_without<std::string>()).has_value());
}
