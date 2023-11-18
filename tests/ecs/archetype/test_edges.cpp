#include <gtest/gtest.h>

#include "ecs/archetype/archetype.h"

TEST(ArchetypeEdges, AddAndGet) {
  auto edges = ArchetypeEdges();
  edges.add<std::string>(1);
  EXPECT_EQ(edges.get<std::string>(), 1);
}

TEST(ArchetypeEdges, MoveConstruct) {
  auto edges1 = ArchetypeEdges();
  edges1.add<int>(1);

  auto edges2 = std::move(edges1);
  EXPECT_TRUE(edges2.get<int>().has_value());
  EXPECT_FALSE(edges1.get<int>().has_value());

  ArchetypeEdges edges3{std::move(edges2)};
  EXPECT_TRUE(edges3.get<int>().has_value());
  EXPECT_FALSE(edges2.get<int>().has_value());
}
