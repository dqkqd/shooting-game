#include <gtest/gtest.h>

#include "ecs/query/query.h"
#include "ecs/world.h"

TEST(Query, MoveQuery) {
  auto world = World();
  auto query1 = std::move(QueryWrapper({1, 2, 3}));
  auto query2 = std::move(query1);
  QueryWrapper query3{std::move(query2)};
}
