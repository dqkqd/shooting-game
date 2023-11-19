#include <gtest/gtest.h>

#include "ecs/archetype/archetypes.h"

TEST(Archetypes, Add) {
  auto archetypes = Archetypes();
  EXPECT_EQ((archetypes.add<int, float, char>()), 0);
  EXPECT_EQ((archetypes.add<int, float, char, double>()), 1);
  EXPECT_EQ(archetypes.size(), 2);

  EXPECT_FALSE((archetypes.add<int, float, char>()).has_value());
  EXPECT_FALSE((archetypes.add<int, float, char, double>()).has_value());
}

TEST(Archetypes, Get) {
  auto archetypes = Archetypes();
  EXPECT_FALSE((archetypes.get<int, float, char>()).has_value());
  EXPECT_EQ((archetypes.add<int, float, char>()), 0);
  EXPECT_EQ((archetypes.get<int, float, char>()), 0);
}

TEST(Archetypes, GetOrAdd) {
  auto archetypes = Archetypes();
  auto id = archetypes.get_or_add<int, float, char>();
  EXPECT_FALSE((archetypes.add<int, float, char>()).has_value());
  EXPECT_EQ((archetypes.get<int, float, char>()), id);
}

TEST(Archetypes, GetOrAddNextArchetype) {
  auto archetypes = Archetypes();
  auto archetype_id = archetypes.get_or_add<int, float>();
  auto next_archetype_id =
      archetypes.get_or_add_next_archetype<char>(archetype_id);
  EXPECT_EQ(archetypes.get_by_id_unchecked(archetype_id).get_next_edge<char>(),
            next_archetype_id);
  EXPECT_EQ(
      archetypes.get_by_id_unchecked(next_archetype_id).get_prev_edge<char>(),
      archetype_id);
}

TEST(Archetypes, GetOrAddPrevArchetype) {
  auto archetypes = Archetypes();
  auto archetype_id = archetypes.get_or_add<int, float, char>();
  auto prev_archetype_id =
      archetypes.get_or_add_prev_archetype<char>(archetype_id);
  EXPECT_EQ(archetypes.get_by_id_unchecked(archetype_id).get_prev_edge<char>(),
            prev_archetype_id);
  EXPECT_EQ(
      archetypes.get_by_id_unchecked(prev_archetype_id).get_next_edge<char>(),
      archetype_id);
}
