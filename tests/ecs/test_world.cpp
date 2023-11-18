#include "ecs/world.h"
#include "gtest/gtest.h"

TEST(World, SpawnEntityWith) {
  auto world = World();
  auto entity_location =
      world.spawn_entity_with<int, std::string>(1, "Hello world");

  EXPECT_EQ((world.archetypes().get<int, std::string>()),
            entity_location.archetype_id);

  auto data = world.archetypes()
                  .get_by_id_unchecked(entity_location.archetype_id)
                  .get_entity_data<std::string>(entity_location.entity_id);
  EXPECT_EQ(data->get(), "Hello world");
}

TEST(World, AddComponentToEntity) {
  auto world = World();
  auto old_location = world.spawn_entity_with<int>(1);
  auto new_location =
      world.add_component_to_entity<float>(old_location.entity_id, 2.0);
  EXPECT_EQ(new_location->entity_id, old_location.entity_id);
  EXPECT_NE(new_location->archetype_id, old_location.archetype_id);
  EXPECT_NE(new_location->table_id, old_location.table_id);

  auto new_data = world.archetypes()
                      .get_by_id_unchecked(new_location->archetype_id)
                      .get_entity_data<float>(new_location->entity_id);
  EXPECT_EQ(new_data->get(), 2.0);

  auto old_data = world.archetypes()
                      .get_by_id_unchecked(old_location.archetype_id)
                      .get_entity_data<int>(old_location.entity_id);
  EXPECT_FALSE(old_data.has_value());
}

TEST(World, AddComponentInvalid) {
  auto world = World();
  auto location = world.spawn_entity_with<int>(1);

  // invalid id
  EXPECT_FALSE(world.add_component_to_entity<float>(location.entity_id + 1, 2.0)
                   .has_value());

  // already existed type
  EXPECT_ANY_THROW(world.add_component_to_entity<int>(location.entity_id, 2));
}

TEST(World, RemoveComponentFromEntity) {
  auto world = World();
  auto old_location = world.spawn_entity_with<int, float>(4, 2.0);
  auto new_location =
      world.remove_component_from_entity<float>(old_location.entity_id);
  EXPECT_EQ(new_location->entity_id, old_location.entity_id);
  EXPECT_NE(new_location->archetype_id, old_location.archetype_id);
  EXPECT_NE(new_location->table_id, old_location.table_id);

  auto new_data = world.archetypes()
                      .get_by_id_unchecked(new_location->archetype_id)
                      .get_entity_data<int>(new_location->entity_id);
  EXPECT_EQ(new_data->get(), 4);

  auto removed_data = world.archetypes()
                          .get_by_id_unchecked(new_location->archetype_id)
                          .get_entity_data<float>(new_location->entity_id);
  EXPECT_FALSE(removed_data.has_value());

  auto old_data = world.archetypes()
                      .get_by_id_unchecked(old_location.archetype_id)
                      .get_entity_data<int>(old_location.entity_id);
  EXPECT_FALSE(old_data.has_value());
}

TEST(World, RemoveComponentInvalid) {
  auto world = World();
  auto location = world.spawn_entity_with<int, float>(1, 2.0);

  // invalid id
  EXPECT_FALSE(world.remove_component_from_entity<float>(location.entity_id + 1)
                   .has_value());

  // not existed type
  EXPECT_ANY_THROW(
      world.remove_component_from_entity<std::string>(location.entity_id));
}

TEST(World, AddQuery) {
  struct A {};

  auto world = World();

  world.spawn_entity_with<int, float, std::string>(1, 2.0, "Hello");
  world.spawn_entity_with<int, float, std::string>(2, 3.0, "from");
  world.spawn_entity_with<int, double, std::string>(3, 4.0, "the");
  world.spawn_entity_with<int, char, std::string>(4, 'x', "other");
  world.spawn_entity_with<int, A, std::string>(5, A{}, "side");

  world.add_query<int, std::string>();

  std::vector<int> is;
  std::vector<std::string> ss;
  for (int _ = 0; _ < 5; ++_) {
    auto [i, s] = world.run_query<int, std::string>(0);
    is.emplace_back(i);
    ss.emplace_back(s);
  }
  EXPECT_EQ(is, std::vector({1, 2, 3, 4, 5}));
  EXPECT_EQ(ss[0], "Hello");
  EXPECT_EQ(ss[1], "from");
  EXPECT_EQ(ss[2], "the");
  EXPECT_EQ(ss[3], "other");
  EXPECT_EQ(ss[4], "side");
}
