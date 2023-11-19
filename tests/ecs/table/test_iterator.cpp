#include <gtest/gtest.h>

#include "ecs/table/table.h"

TEST(TableIterator, Basic) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);
  table.add_row<int, float>(3, 4.0);

  auto iter = table.begin<int, float>();

  // get values
  auto [i1, f1] = *iter;
  EXPECT_EQ(i1, 1);
  EXPECT_EQ(f1, 2.0);

  // advance then get value
  ++iter;
  auto [i2, f2] = *iter;
  EXPECT_EQ(i2, 3);
  EXPECT_EQ(f2, 4.0);

  // value could be modified
  i2 = 5;
  f2 = 8.0;
  auto [i3, f3] = *iter;
  EXPECT_EQ(i3, 5);
  EXPECT_EQ(f3, 8.0);

  ++iter;
  // iter is now done
  EXPECT_TRUE(iter.done());

  // iter could be looped
  std::vector<int> is;
  std::vector<float> fs;
  for (auto it = table.begin<int, float>();  // NOLINT
       it != table.end<int, float>(); ++it) {
    auto [u, v] = *it;
    is.push_back(u);
    fs.push_back(v);
  }
  EXPECT_EQ(is, std::vector({1, 5}));
  EXPECT_EQ(fs, std::vector({2.0F, 8.0F}));

  // iter could be subset of type
  auto [i4] = *table.begin<int>();
  EXPECT_EQ(i4, 1);
  auto [f4] = *table.begin<float>();
  EXPECT_EQ(f4, 2.0F);
}
