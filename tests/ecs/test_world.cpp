#include "ecs/world.h"
#include "gtest/gtest.h"

TEST(World, SpawnEntityWith) {
  auto world = World();
  auto entity_location = world.spawn_entity_with<int, std::string>(1, "World");
  EXPECT_EQ(entity_location.entity_id, 0);
  EXPECT_EQ(entity_location.archetype_id, 0);
  EXPECT_EQ(entity_location.table_row, 0);
}

TEST(World, AddComponentToEntity) {
  auto world = World();
  auto old_location = world.spawn_entity_with<int>(1);
  auto new_location =
      world.add_component_to_entity<float>(old_location.entity_id, 2.0);
  EXPECT_EQ(new_location->entity_id, old_location.entity_id);
  EXPECT_NE(new_location->archetype_id, old_location.archetype_id);
  EXPECT_NE(new_location->table_id, old_location.table_id);
  // TODO(khanhdq): add more test getting value after implementing query
}

TEST(World, AddComponentInvalid) {
  auto world = World();
  auto location = world.spawn_entity_with<int>(1);

  // invalid id
  EXPECT_FALSE(world.add_component_to_entity<float>(location.entity_id + 1, 2.0)
                   .has_value());

  // already existed type
  EXPECT_FALSE(
      world.add_component_to_entity<int>(location.entity_id, 2).has_value());
}

TEST(World, RemoveComponentFromEntity) {
  auto world = World();
  auto old_location = world.spawn_entity_with<int, float>(1, 2.0);
  auto new_location =
      world.remove_component_from_entity<float>(old_location.entity_id);
  EXPECT_EQ(new_location->entity_id, old_location.entity_id);
  EXPECT_NE(new_location->archetype_id, old_location.archetype_id);
  EXPECT_NE(new_location->table_id, old_location.table_id);
  // TODO(khanhdq): add more test getting value after implementing query
}

TEST(World, RemoveComponentInvalid) {
  auto world = World();
  auto location = world.spawn_entity_with<int, float>(1, 2.0);

  // invalid id
  EXPECT_FALSE(world.remove_component_from_entity<float>(location.entity_id + 1)
                   .has_value());

  // not existed type
  EXPECT_FALSE(
      world.remove_component_from_entity<std::string>(location.entity_id)
          .has_value());
}
