#include <gtest/gtest.h>

#include "ecs/archetype/archetype_components.h"
#include "ecs/column.h"

TEST(ArchetypeComponents, MoveConstructor) {
  auto components =
      ArchetypeComponents::create_archetype_components<int, float,
                                                       std::string>();
  auto components2 = std::move(components);
  EXPECT_EQ(components2.size(), 3);
  EXPECT_EQ(components.size(), 0);
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
