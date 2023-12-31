#include <gtest/gtest.h>

#include "ecs/column/column.h"
#include "ecs/component.h"

class ComponentsTest : public testing::Test {
 protected:
  struct A {};

  Components components;
  void SetUp() override { components = Components::from_types<int, float>(); }
};

TEST_F(ComponentsTest, HasComponent) {
  EXPECT_TRUE(components.has_components<int>());
  EXPECT_TRUE(components.has_components<float>());
  EXPECT_TRUE((components.has_components<int, float>()));

  EXPECT_FALSE(components.has_components<char>());
  EXPECT_FALSE((components.has_components<int, char>()));
  EXPECT_FALSE(components.has_components());
}

TEST_F(ComponentsTest, MoveConstructor) {
  auto components2 = std::move(components);
  EXPECT_EQ(components2.size(), 2);
  EXPECT_EQ(components.size(), 0);
}

TEST_F(ComponentsTest, Comparison) {
  EXPECT_EQ(components, ((Components::from_types<float, int>())));
  EXPECT_LT(components, ((Components::from_types<float, int, double>())));
}

TEST_F(ComponentsTest, VecAndSetConstruct) {
  EXPECT_EQ(components, Components::from_vec({ComponentCounter::id<int>(),
                                              ComponentCounter::id<float>()}));
  EXPECT_EQ(components, Components::from_set({ComponentCounter::id<int>(),
                                              ComponentCounter::id<float>()}));
}

TEST_F(ComponentsTest, Clone) {
  EXPECT_EQ(components.clone(), (Components::from_types<float, int>()));
  EXPECT_EQ(components.clone(), components);
}

TEST_F(ComponentsTest, CloneWith) {
  auto cloned = components.clone_with<A>();
  EXPECT_EQ(cloned, (Components::from_types<int, float, A>()));
  // existed component
  EXPECT_FALSE((components.clone_with<float>()).has_value());
}

TEST_F(ComponentsTest, CloneWithout) {
  auto cloned = components.clone_without<int>();
  EXPECT_EQ(cloned, (Components::from_types<float>()));
  // non existed component
  EXPECT_FALSE((components.clone_without<A>()).has_value());
}

TEST_F(ComponentsTest, Cover) {
  EXPECT_TRUE(components.cover(Components::from_types<float, int>()));
  EXPECT_TRUE(components.cover(Components::from_types<int, float>()));
  EXPECT_TRUE(components.cover(Components::from_types<int>()));
  EXPECT_TRUE(components.cover(Components::from_types<float>()));
  EXPECT_TRUE((Components::from_types<float, int, double>()).cover(components));
  EXPECT_TRUE((Components::from_types<char, float, int>()).cover(components));

  EXPECT_FALSE((Components::from_types<char, float>()).cover(components));
}
