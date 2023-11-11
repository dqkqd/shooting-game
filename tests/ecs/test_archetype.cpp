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
