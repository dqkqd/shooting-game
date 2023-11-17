#include <gtest/gtest.h>

#include "ecs/archetype/archetypes.h"

TEST(Archetypes, Add) {
  auto archetypes = Archetypes();
  EXPECT_EQ((archetypes.add<int, float, std::string>()), 0);
  EXPECT_EQ((archetypes.add<int, float, std::string, double>()), 1);
  EXPECT_EQ(archetypes.size(), 2);

  EXPECT_FALSE((archetypes.add<int, float, std::string>()).has_value());
  EXPECT_FALSE((archetypes.add<int, float, std::string, double>()).has_value());
}

TEST(Archetypes, Get) {
  auto archetypes = Archetypes();
  EXPECT_FALSE((archetypes.get<int, float, std::string>()).has_value());
  EXPECT_EQ((archetypes.add<int, float, std::string>()), 0);
  EXPECT_EQ((archetypes.get<int, float, std::string>()), 0);
}

TEST(Archetypes, GetOrAdd) {
  auto archetypes = Archetypes();
  auto id = archetypes.get_or_add<int, float, std::string>();
  EXPECT_FALSE((archetypes.add<int, float, std::string>()).has_value());
  EXPECT_EQ((archetypes.get<int, float, std::string>()), id);
}

TEST(Archetypes, GetOrAddNextArchetype) {
  auto archetypes = Archetypes();
  auto archetype_id = archetypes.get_or_add<int, float>();
  auto next_archetype_id =
      archetypes.get_or_add_next_archetype<std::string>(archetype_id);
  EXPECT_EQ(
      archetypes.get_by_id_unchecked(archetype_id).get_next_edge<std::string>(),
      next_archetype_id);
  EXPECT_EQ(archetypes.get_by_id_unchecked(next_archetype_id)
                .get_prev_edge<std::string>(),
            archetype_id);
}

TEST(Archetypes, GetOrAddPrevArchetype) {
  auto archetypes = Archetypes();
  auto archetype_id = archetypes.get_or_add<int, float, std::string>();
  auto prev_archetype_id =
      archetypes.get_or_add_prev_archetype<std::string>(archetype_id);
  EXPECT_EQ(
      archetypes.get_by_id_unchecked(archetype_id).get_prev_edge<std::string>(),
      prev_archetype_id);
  EXPECT_EQ(archetypes.get_by_id_unchecked(prev_archetype_id)
                .get_next_edge<std::string>(),
            archetype_id);
}
