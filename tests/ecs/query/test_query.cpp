#include <gtest/gtest.h>

#include "ecs/query/query.h"
#include "ecs/world.h"

class QueryTest : public testing::Test {
 protected:
  World world;
  Queries queries;

  EntityLocation location1;
  EntityLocation location2;

  void SetUp() override {
    location1 = world.spawn_entity_with<int, float>(2, 2.0);
    location2 = world.spawn_entity_with<int, float>(3, 3.0);
  }

  template <typename T>
  auto get_values(Query<T>&& query) -> std::vector<T> {
    std::vector<T> values;
    for (auto [v] : std::move(query)) {
      values.push_back(v);
    }
    return values;
  };

  template <typename T>
  auto get_values(QueryWrapper& wrapper) -> std::vector<T> {
    return get_values<T>(wrapper.query<T>(world.archetypes()));
  };
};

TEST_F(QueryTest, MoveQueryWrapper) {
  auto query1 = QueryWrapper({location1.archetype_id});
  EXPECT_EQ(get_values<int>(query1), std::vector({2, 3}));

  auto query2 = std::move(query1);
  EXPECT_EQ(get_values<int>(query1), std::vector<int>({}));
  EXPECT_EQ(get_values<int>(query2), std::vector({2, 3}));

  QueryWrapper query3{std::move(query2)};
  EXPECT_EQ(get_values<int>(query2), std::vector<int>({}));
  EXPECT_EQ(get_values<int>(query3), std::vector({2, 3}));
}

TEST_F(QueryTest, AddOrGetQueryFromQueries) {
  world.spawn_entity_with<char>('c');
  auto query = queries.get<char>(world.archetypes());
  EXPECT_EQ(get_values<char>(std::move(query)), std::vector({'c'}));

  EXPECT_EQ(queries.size(), 1);
  queries.get<char>(world.archetypes());
  queries.get<char>(world.archetypes());
  queries.get<char>(world.archetypes());

  EXPECT_EQ(queries.size(), 1);
}

TEST_F(QueryTest, AddArchetypeToQueryWrapper) {
  auto query = QueryWrapper({location1.archetype_id});
  auto location = world.spawn_entity_with<int>(100);
  EXPECT_EQ(get_values<int>(query), std::vector({2, 3}));

  query.add_archetype(location.archetype_id);
  EXPECT_EQ(get_values<int>(query), std::vector({2, 3, 100}));
}
