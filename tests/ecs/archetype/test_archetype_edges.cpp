#include <gtest/gtest.h>

#include "ecs/archetype/archetype.h"
#include "ecs/archetype/archetype_edges.h"

TEST(ArchetypeEdges, AddAndGet) {
  auto edges = ArchetypeEdges();
  auto archetype = Archetype::create_archetype<int, float>(0);
  edges.add<std::string>(1);
  EXPECT_EQ(edges.get<std::string>(), 1);
}
