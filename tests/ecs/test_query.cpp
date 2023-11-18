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
  world.spawn_entity_with<int, float, std::string>(1, 2.0, "Hello");
  world.spawn_entity_with<int, float, std::string>(3, 4.0, "World");

  auto query = world.query<int, float, std::string>();
  EXPECT_FALSE(query.done());

  auto [i1, f1, s1] = query.next();
  EXPECT_EQ(i1, 1);
  EXPECT_EQ(f1, 2.0);
  EXPECT_EQ(s1, "Hello");

  auto [i2, f2, s2] = query.next();
  EXPECT_EQ(i2, 3);
  EXPECT_EQ(f2, 4.0);
  EXPECT_EQ(s2, "World");

  EXPECT_TRUE(query.done());
}

TEST(Query, QueryAcrossArchetypes) {
  struct A {};
  auto world = World();
  world.spawn_entity_with<int, float, std::string>(1, 2.0, "Hello");
  world.spawn_entity_with<int, float, std::string>(2, 3.0, "from");
  world.spawn_entity_with<int, double, std::string>(3, 4.0, "the");
  world.spawn_entity_with<int, char, std::string>(4, 'x', "other");
  world.spawn_entity_with<int, A, std::string>(5, A{}, "side");

  auto query = world.query<int, std::string>();
  std::vector<int> is;
  std::vector<std::string> ss;
  while (!query.done()) {
    auto [i, s] = query.next();
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
