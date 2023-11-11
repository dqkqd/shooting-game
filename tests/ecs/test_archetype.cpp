#include <gtest/gtest.h>

#include <stdexcept>

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

TEST(Archetype, AddEntity) {
  auto archetype = Archetype::create_archetype<int, float>();
  auto location = archetype.add_entity<int, float>(10, 1, 2.0);
  EXPECT_EQ(location.archetype_id, archetype.archetype_id());
  EXPECT_EQ(location.table_id, archetype.table_id());
  EXPECT_EQ(location.row, 0);
}

TEST(Archetype, MoveConstructor) {
  auto archetype1 = Archetype::create_archetype<int, float>();
  archetype1.add_entity<int, float>(10, 1, 2.0);
  archetype1.add_entity<int, float>(20, 2, 3.0);

  auto archetype2 = std::move(archetype1);
  EXPECT_FALSE(archetype2.is_empty());
  EXPECT_TRUE((archetype2.has_components<int, float>()));
  EXPECT_NE(archetype2.archetype_id(), INVALID_ARCHETYPE_ID);
  EXPECT_TRUE(archetype1.is_empty());
  EXPECT_FALSE((archetype1.has_components<int, float>()));
  EXPECT_EQ(archetype1.archetype_id(), INVALID_ARCHETYPE_ID);

  Archetype archetype3{std::move(archetype2)};
  EXPECT_FALSE(archetype3.is_empty());
  EXPECT_TRUE((archetype3.has_components<int, float>()));
  EXPECT_NE(archetype3.archetype_id(), INVALID_ARCHETYPE_ID);
  EXPECT_TRUE(archetype2.is_empty());
  EXPECT_FALSE((archetype2.has_components<int, float>()));
  EXPECT_EQ(archetype2.archetype_id(), INVALID_ARCHETYPE_ID);
}

TEST(Archetype, EntityLocation) {
  auto archetype = Archetype::create_archetype<int, float>();

  EXPECT_FALSE(archetype.location(10).has_value());
  archetype.add_entity<int, float>(10, 1, 2.0);
  EXPECT_TRUE(archetype.location(10).has_value());
  EXPECT_EQ(archetype.location(10)->row, 0);  // NOLINT

  EXPECT_FALSE(archetype.location(20).has_value());
  archetype.add_entity<int, float>(20, 3, 4.0);
  EXPECT_TRUE(archetype.location(20).has_value());
  EXPECT_EQ(archetype.location(20)->row, 1);  // NOLINT
}

TEST(Archetype, MoveEntitySuccessful) {
  auto archetype = Archetype::create_archetype<int, float>();
  archetype.add_entity<int, float>(10, 1, 2.0);
  archetype.add_entity<int, float>(20, 3, 4.0);

  auto other = Archetype::create_archetype<int, float>();
  auto location = archetype.move_entity_to_other(10, other);

  EXPECT_EQ(location->row, 0);  // NOLINT
  EXPECT_FALSE(archetype.location(10).has_value());
  EXPECT_TRUE(other.location(10).has_value());

  EXPECT_TRUE(archetype.is_valid());
  EXPECT_TRUE(other.is_valid());
}

TEST(Archetype, MoveEntityIsNotOverlappedWithoutEnoughArguments) {
  auto archetype = Archetype::create_archetype<int, float>();
  archetype.add_entity<int, float>(10, 1, 2.0);
  archetype.add_entity<int, float>(20, 3, 4.0);

  auto other = Archetype::create_archetype<std::string>();
  EXPECT_THROW(archetype.move_entity_to_other(10, other), std::runtime_error);
}

TEST(Archetype, MoveEntitySubset) {
  auto archetype = Archetype::create_archetype<int, float>();
  archetype.add_entity<int, float>(10, 1, 2.0);
  archetype.add_entity<int, float>(20, 3, 4.0);

  auto other = Archetype::create_archetype<int>();
  auto location = archetype.move_entity_to_other(10, other);

  EXPECT_EQ(location->row, 0);  // NOLINT
  EXPECT_FALSE(archetype.location(10).has_value());
  EXPECT_TRUE(other.location(10).has_value());

  EXPECT_TRUE(archetype.is_valid());
  EXPECT_TRUE(other.is_valid());
}

TEST(Archetype, MoveEntityOverlappedWithAdditionalArguments) {
  auto archetype = Archetype::create_archetype<int, float>();
  archetype.add_entity<int, float>(10, 1, 2.0);
  archetype.add_entity<int, float>(20, 3, 4.0);

  auto other = Archetype::create_archetype<int, double>();
  auto location = archetype.move_entity_to_other<double>(10, other, 5.0);

  EXPECT_EQ(location->row, 0);  // NOLINT
  EXPECT_FALSE(archetype.location(10).has_value());
  EXPECT_TRUE(other.location(10).has_value());

  EXPECT_TRUE(archetype.is_valid());

  // double component is added
  EXPECT_TRUE(other.is_valid());
}
