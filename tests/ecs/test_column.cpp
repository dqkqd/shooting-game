#include <gtest/gtest.h>

#include "ecs/column.h"

TEST(Column, BasicConstruct) {
  auto int_column = Column::create_column<int>();
  EXPECT_TRUE(int_column.is<int>());
  EXPECT_FALSE(int_column.is<float>());
  EXPECT_FALSE(int_column.is<std::string>());
  EXPECT_FALSE(int_column.is<size_t>());
}

TEST(Column, AddElement) {
  auto int_column = Column::create_column<int>();

  int_column.push(15);
  int_column.push(45);
  int_column.push(30);
  EXPECT_EQ(int_column.size(), 3);

  EXPECT_EQ(int_column.get_data_unchecked<int>(0), 15);
  EXPECT_EQ(int_column.get_data_unchecked<int>(1), 45);
  EXPECT_EQ(int_column.get_data_unchecked<int>(2), 30);
  EXPECT_EQ(int_column.get_data<int>(0)->get(), 15);  // NOLINT
  EXPECT_EQ(int_column.get_data<int>(1)->get(), 45);  // NOLINT
  EXPECT_EQ(int_column.get_data<int>(2)->get(), 30);  // NOLINT

  // add non integer element
  EXPECT_ANY_THROW(int_column.push(1.5));
  EXPECT_ANY_THROW(int_column.push("hello"));
  EXPECT_ANY_THROW(int_column.push(static_cast<size_t>(1)));
}

TEST(Column, AddTooManyElement) {
  auto int_column = Column::create_column<int>();
  for (int i = 0; i < 1000; ++i) {
    int_column.push(i);
  }
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(int_column.get_data_unchecked<int>(i), i);
  }
}

TEST(Column, ReassignElement) {
  auto int_column = Column::create_column<int>();
  int_column.push(10);
  int_column.push(20);

  int_column.get_data_unchecked<int>(0) = 30;
  int_column.get_data_unchecked<int>(1) = 40;

  EXPECT_EQ(int_column.get_data_unchecked<int>(0), 30);
  EXPECT_EQ(int_column.get_data_unchecked<int>(1), 40);
}