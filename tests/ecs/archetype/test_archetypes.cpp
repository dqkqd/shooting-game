#include <gtest/gtest.h>

#include "ecs/archetype/archetypes.h"

TEST(Archetypes, Add) {
  auto archetypes = Archetypes();
  EXPECT_TRUE((archetypes.add<int, float, std::string>()).has_value());
  EXPECT_TRUE((archetypes.add<int, float, std::string, double>()).has_value());
  EXPECT_EQ(archetypes.size(), 2);

  EXPECT_FALSE((archetypes.add<int, float, std::string>()).has_value());
  EXPECT_FALSE((archetypes.add<int, float, std::string, double>()).has_value());
}

TEST(Archetypes, Get) {
  auto archetypes = Archetypes();
  EXPECT_FALSE((archetypes.get<int, float, std::string>()).has_value());
  EXPECT_TRUE((archetypes.add<int, float, std::string>()).has_value());
  EXPECT_TRUE((archetypes.get<int, float, std::string>()).has_value());
}

TEST(Archetypes, GetOrAdd) {
  auto archetypes = Archetypes();
  auto id = archetypes.get_or_add<int, float, std::string>().archetype_id();
  EXPECT_FALSE((archetypes.add<int, float, std::string>()).has_value());

  EXPECT_EQ((archetypes  // NOLINT
                 .get<int, float, std::string>())
                ->get()
                .archetype_id(),
            id);
}
