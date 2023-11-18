#include <gtest/gtest.h>

#include "ecs/query.h"

class QueryTest : public testing::Test {
 protected:
  template <typename... Args>
  auto create_query(World &world) -> Query<Args...> {
    return Query<Args...>(world);
  }
};

TEST_F(QueryTest, Next) {
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
