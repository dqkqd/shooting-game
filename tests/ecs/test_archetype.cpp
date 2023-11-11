#include <gtest/gtest.h>

#include "ecs/archetype.h"
#include "ecs/column.h"
#include "ecs/table.h"

TEST(Archetype, Constructor) {
  auto table = Table();
  table.add_column(Column::create_column<int>());
  table.add_column(Column::create_column<float>());
  auto archetype = Archetype(std::move(table));
}

TEST(Archetype, InstanceShouldDifferent) {
  auto archetype1 = Archetype(Table());
  auto archetype2 = Archetype(Table());
  EXPECT_NE(archetype1.archetype_id(), archetype2.archetype_id());
}

TEST(Archetype, HasComponents) {
  struct A1 {};
  struct A2 {};
  struct A3 {};

  auto table = Table();
  table.add_column(Column::create_column<A1>());
  table.add_column(Column::create_column<A2>());

  auto archetype = Archetype(std::move(table));
  auto has_a1 = archetype.has_components<A1>();
  auto has_a1_a2 = archetype.has_components<A1, A2>();
  auto has_a1_a2_a3 = archetype.has_components<A1, A2, A3>();

  EXPECT_TRUE(has_a1);
  EXPECT_TRUE(has_a1_a2);
  EXPECT_FALSE(has_a1_a2_a3);
}

TEST(Archetype, ComponentsMustBeSorted) {
  struct A1 {};
  struct A2 {};
  struct A3 {};

  auto table = Table();
  table.add_column(Column::create_column<A3>());
  table.add_column(Column::create_column<A1>());
  table.add_column(Column::create_column<A2>());
  auto archetype = Archetype(std::move(table));
  std::vector expected{ColumnCounter::id<A1>(), ColumnCounter::id<A2>(),
                       ColumnCounter::id<A3>()};
  std::sort(expected.begin(), expected.end());
  EXPECT_EQ(archetype.components(), expected);
}
