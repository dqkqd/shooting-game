#include "ecs/world.h"
#include "gtest/gtest.h"

TEST(World, SpawnEntityWith) {
  auto world = World();
  auto entity_location = world.spawn_entity_with<int, std::string>(1, "World");
  EXPECT_EQ(entity_location.entity_id, 0);
  EXPECT_EQ(entity_location.archetype_id, 0);
  EXPECT_EQ(entity_location.table_row, 0);
}
