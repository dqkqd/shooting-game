#include <gtest/gtest.h>

#include "ecs/query/query.h"
#include "ecs/world.h"

TEST(Query, MoveQuery) {
  auto world = World();
  auto query1 = std::move(Query(world.archetypes(), {1, 2, 3}));
  auto query2 = std::move(query1);
  Query query3{std::move(query2)};
}

TEST(Query, QueryWithinTheSameArchetype) {
  auto world = World();
  world.spawn_entity_with<int, float, char>(1, 2.0, 'a');
  world.spawn_entity_with<int, float, char>(3, 4.0, 'b');

  auto query = world.query<int, float, char>();
  EXPECT_FALSE(query.done());

  auto [i1, f1, s1] = query.next();
  EXPECT_EQ(i1, 1);
  EXPECT_EQ(f1, 2.0);
  EXPECT_EQ(s1, 'a');

  auto [i2, f2, s2] = query.next();
  EXPECT_EQ(i2, 3);
  EXPECT_EQ(f2, 4.0);
  EXPECT_EQ(s2, 'b');

  EXPECT_TRUE(query.done());
}

TEST(Query, QueryAcrossArchetypes) {
  struct A {};
  auto world = World();
  world.spawn_entity_with<int, float, char>(1, 2.0, 'a');
  world.spawn_entity_with<int, float, char>(2, 3.0, 'b');
  world.spawn_entity_with<int, double, char>(3, 4.0, 'c');
  world.spawn_entity_with<int, size_t, char>(4, 5, 'd');
  world.spawn_entity_with<int, A, char>(5, A{}, 'e');

  auto query = world.query<int, char>();
  std::vector<int> is;
  std::vector<char> ss;
  while (!query.done()) {
    auto [i, s] = query.next();
    is.emplace_back(i);
    ss.emplace_back(s);
  }
  EXPECT_EQ(is, std::vector({1, 2, 3, 4, 5}));
  EXPECT_EQ(ss, std::vector({'a', 'b', 'c', 'd', 'e'}));
}
