#include <gtest/gtest.h>

#include "ecs/column.h"
#include "ecs/component.h"

TEST(Components, MoveConstructor) {
  auto components = Components::from_types<int, float, std::string>();
  auto components2 = std::move(components);
  EXPECT_EQ(components2.size(), 3);
  EXPECT_EQ(components.size(), 0);
}

TEST(Components, Comparison) {
  EXPECT_EQ((Components::from_types<int, float>()),
            ((Components::from_types<float, int>())));
  EXPECT_LT((Components::from_types<int, float>()),
            ((Components::from_types<float, int, double>())));
}

TEST(Components, VecAndSetConstruct) {
  EXPECT_EQ((Components::from_types<int, float>()),
            Components::from_vec(
                {ComponentCounter::id<int>(), ComponentCounter::id<float>()}));
  EXPECT_EQ((Components::from_types<int, float>()),
            Components::from_set(
                {ComponentCounter::id<int>(), ComponentCounter::id<float>()}));
}

TEST(Components, Clone) {
  auto components = Components::from_types<int, float, char>();
  EXPECT_EQ(components.clone(), (Components::from_types<int, float, char>()));
  EXPECT_EQ(components.clone(), components);
}

TEST(Components, CloneWith) {
  auto components = Components::from_types<int, float, char>();
  auto cloned = components.clone_with<std::string>();
  EXPECT_EQ(cloned, (Components::from_types<int, float, char, std::string>()));
  // existed component
  EXPECT_FALSE((components.clone_with<float>()).has_value());
}

TEST(Components, CloneWithout) {
  auto components = Components::from_types<int, float, char>();
  auto cloned = components.clone_without<int, char>();
  EXPECT_EQ(cloned, (Components::from_types<float>()));
  // non existed component
  EXPECT_FALSE((components.clone_without<std::string>()).has_value());
}
