#include "ecs/query/query.h"
#include "ecs/world.h"
#include "gtest/gtest.h"

TEST(World, SpawnEntityWith) {
  auto world = World();
  auto entity_location = world.spawn_entity_with<int, char>(1, 'a');

  EXPECT_EQ((world.archetypes().get<int, char>()),
            entity_location.archetype_id);

  auto data = world.archetypes()
                  .get_by_id_unchecked(entity_location.archetype_id)
                  .get_entity_data<char>(entity_location.entity_id);
  EXPECT_EQ(data, std::make_tuple('a'));
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
  EXPECT_EQ(new_data, std::make_tuple(2.0));

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
  EXPECT_EQ(new_data, std::make_tuple(4));

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
      world.remove_component_from_entity<char>(location.entity_id));
}

void test_system(Query<int, float> q) {
  for (auto [i, c] : q) {
    i *= 2;
    c *= 3;
  }
}

TEST(World, SystemOverSingleArchetype) {
  auto world = World();
  auto location = world.spawn_entity_with<int, float>(1, 1.0);
  world.add_system(test_system);

  world.run_systems();
  EXPECT_EQ(world.archetypes()
                .get_by_id_unchecked(location.archetype_id)
                .get_entity_data<int>(location.entity_id),
            std::make_tuple(2));

  world.run_systems();
  EXPECT_EQ(world.archetypes()
                .get_by_id_unchecked(location.archetype_id)
                .get_entity_data<int>(location.entity_id),
            std::make_tuple(4));
}

TEST(World, SystemAcrossMultipleArchetypes) {
  auto world = World();
  auto location = world.spawn_entity_with<int, float, char>(2, 2.0, 'a');
  world.spawn_entity_with<int, float>(1, 1.0);

  auto system = [](Query<int, float> query) {
    for (auto [i, _] : query) {
      i += 2;
    }
  };

  world.add_system(*system);

  world.run_systems();
  EXPECT_EQ(world.archetypes()
                .get_by_id_unchecked(location.archetype_id)
                .get_entity_data<int>(location.entity_id),
            std::make_tuple(4));

  world.run_systems();
  EXPECT_EQ(world.archetypes()
                .get_by_id_unchecked(location.archetype_id)
                .get_entity_data<int>(location.entity_id),
            std::make_tuple(6));
}

TEST(World, WorldQueryAutoUpdateWhenArchetypeChanged) {
  auto world = World();
  world.spawn_entity_with<int, float>(1, 1.0);

  auto system = [](Query<int> query) {
    for (auto [i] : query) {
      i *= 2;
    }
  };

  world.add_system(*system);
  auto location = world.spawn_entity_with<int>(10);

  world.run_systems();
  EXPECT_EQ(world.archetypes()
                .get_by_id_unchecked(location.archetype_id)
                .get_entity_data<int>(location.entity_id),
            std::make_tuple(20));

  world.run_systems();
  EXPECT_EQ(world.archetypes()
                .get_by_id_unchecked(location.archetype_id)
                .get_entity_data<int>(location.entity_id),
            std::make_tuple(40));
}

TEST(World, GetQuery) {
  auto world = World();
  world.spawn_entity_with<int, float>(1, 1.0);
  world.spawn_entity_with<int>(2);

  std::vector<int> res;
  for (auto [v] : world.query<int>()) {
    res.push_back(v);
  }

  EXPECT_EQ(res, std::vector({1, 2}));
}

TEST(World, WorldSystemsShouldRunInOrder) {
  auto world = World();
  world.spawn_entity_with<int>(1);
  world.spawn_entity_with<int>(2);

  auto sequential_system = [](Query<int> query) {
    for (auto [i] : query) {
      i *= 2;
    }
  };
  auto parallel_system = [](Query<int> query) {
    for (auto [i] : query) {
      i += 1;
    }
  };

  world
      .add_system(*sequential_system)         // final result: i = i * 2
      .add_system(*sequential_system)         // final result: i = i * 4
      .add_parallel_system(*parallel_system)  //
      .add_parallel_system(*parallel_system)  //
      .add_parallel_system(*parallel_system)  //
      .add_parallel_system(*parallel_system)  // final result: i = 4 * i + 4
      .add_system(*sequential_system)         // final result: i = 8 * i + 8
      ;

  world.run_systems();

  std::vector<int> res;
  for (auto [v] : world.query<int>()) {
    res.push_back(v);
  }

  EXPECT_EQ(res, std::vector({16, 24}));
}
