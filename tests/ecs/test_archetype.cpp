#include <gtest/gtest.h>

#include "ecs/archetype.h"
#include "ecs/column.h"
#include "ecs/primitive.h"
#include "ecs/table.h"

TEST(Archetype, Constructor) {
  auto archetype = Archetype::create_archetype<int, float>();
}

TEST(Archetype, InstanceShouldDifferent) {
  auto archetype1 = Archetype::create_archetype<int>();
  auto archetype2 = Archetype::create_archetype<int>();
  EXPECT_NE(archetype1.archetype_id(), archetype2.archetype_id());
}

TEST(Archetype, HasComponents) {
  struct A1 {};
  struct A2 {};
  struct A3 {};

  auto archetype = Archetype::create_archetype<A1, A2>();
  EXPECT_TRUE((archetype.has_components<A1>()));
  EXPECT_TRUE((archetype.has_components<A1, A2>()));
  EXPECT_FALSE((archetype.has_components<A1, A2, A3>()));
}

TEST(Archetype, ComponentsMustBeSorted) {
  struct A1 {};
  struct A2 {};
  struct A3 {};

  auto archetype = Archetype::create_archetype<A1, A2, A3>();
  std::vector expected{ColumnCounter::id<A1>(), ColumnCounter::id<A2>(),
                       ColumnCounter::id<A3>()};
  std::sort(expected.begin(), expected.end());
  EXPECT_EQ(archetype.components(), expected);
}

TEST(Archetype, MoveConstructor) {
  auto archetype1 = Archetype::create_archetype<int, float>();

  // TODO(khanhdq) check empty after implementing add entities
  auto archetype2 = std::move(archetype1);
  EXPECT_TRUE((archetype2.has_components<int, float>()));
  EXPECT_NE(archetype2.archetype_id(), INVALID_ARCHETYPE_ID);
  EXPECT_FALSE((archetype1.has_components<int, float>()));
  EXPECT_EQ(archetype1.archetype_id(), INVALID_ARCHETYPE_ID);

  // TODO(khanhdq) check empty after implementing add entities
  Archetype archetype3{std::move(archetype2)};
  EXPECT_TRUE((archetype3.has_components<int, float>()));
  EXPECT_NE(archetype3.archetype_id(), INVALID_ARCHETYPE_ID);
  EXPECT_FALSE((archetype2.has_components<int, float>()));
  EXPECT_EQ(archetype2.archetype_id(), INVALID_ARCHETYPE_ID);
}