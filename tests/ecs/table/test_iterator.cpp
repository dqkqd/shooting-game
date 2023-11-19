#include <gtest/gtest.h>

#include "ecs/table/table.h"

class TableIteratorTest : public testing::Test {
 protected:
  Table table = Table::create_table<int, float>();

  void SetUp() override {
    table.add_row<int, float>(1, 2.0);
    table.add_row<int, float>(3, 4.0);
  }
};

TEST_F(TableIteratorTest, Get) {
  auto iter = this->table.begin<int, float>();

  auto [i, f] = *iter;
  EXPECT_EQ(i, 1);
  EXPECT_EQ(f, 2.0);
}

TEST_F(TableIteratorTest, GetSubset) {
  auto iter = this->table.begin<int>();

  auto [i] = *iter;
  EXPECT_EQ(i, 1);
}

TEST_F(TableIteratorTest, Advance) {
  auto iter = this->table.begin<int, float>();

  ++iter;
  auto [i, f] = *iter;
  EXPECT_EQ(i, 3);
  EXPECT_EQ(f, 4.0);
}

TEST_F(TableIteratorTest, Modify) {
  auto iter = this->table.begin<int, float>();

  auto [i, f] = *iter;
  i = 10;
  f = 20;

  auto iter2 = this->table.begin<int, float>();
  auto [i2, f2] = *iter;
  EXPECT_EQ(i2, 10);
  EXPECT_EQ(f2, 20);
}

TEST_F(TableIteratorTest, Done) {
  auto iter = this->table.begin<int, float>();
  iter++;
  iter++;
  EXPECT_TRUE(iter.done());
}

TEST_F(TableIteratorTest, Loop) {
  std::vector<int> is;
  std::vector<float> fs;
  for (auto [u, v] : table.iter<int, float>()) {
    is.push_back(u);
    fs.push_back(v);
  }
  EXPECT_EQ(is, std::vector({1, 3}));
  EXPECT_EQ(fs, std::vector({2.0F, 4.0F}));
}
