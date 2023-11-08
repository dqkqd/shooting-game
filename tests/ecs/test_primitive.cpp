#include <gtest/gtest.h>

#include "ecs/primitive.h"

TEST(ComponentIdCounter, ComponentIdShouldDifferentBetweenTypes) {
  auto component_id_int = ComponentIdCounter::get<int>();
  auto component_id_float = ComponentIdCounter::get<float>();

  auto component_id_int2 = ComponentIdCounter::get<int>();
  auto component_id_float2 = ComponentIdCounter::get<float>();

  EXPECT_EQ(component_id_int, component_id_int2);
  EXPECT_EQ(component_id_float, component_id_float2);
  EXPECT_NE(component_id_int, component_id_float);
  EXPECT_NE(component_id_int2, component_id_float2);
}