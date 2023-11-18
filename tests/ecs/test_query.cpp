#include <gtest/gtest.h>

#include "ecs/query/query.h"

class QueryTest : public testing::Test {
 protected:
  template <typename... Args>
  auto create_query(World &world) -> Query<Args...> {
    return Query<Args...>(world);
  }
};

TEST_F(QueryTest, QueryWithinTheSameArchetype) {
  auto world = World();
  world.spawn_entity_with<int, float, std::string>(1, 2.0, "Hello");
  world.spawn_entity_with<int, float, std::string>(3, 4.0, "World");
  auto query = this->create_query<int, float, std::string>(world);

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

TEST_F(QueryTest, QueryAcrossArchetypes) {
  struct A {};
  auto world = World();
  world.spawn_entity_with<int, float, std::string>(1, 2.0, "Hello");
  world.spawn_entity_with<int, float, std::string>(2, 3.0, "from");
  world.spawn_entity_with<int, double, std::string>(3, 4.0, "the");
  world.spawn_entity_with<int, char, std::string>(4, 'x', "other");
  world.spawn_entity_with<int, A, std::string>(5, A{}, "side");

  auto query = this->create_query<int, std::string>(world);
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
