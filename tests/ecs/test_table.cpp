#include <gtest/gtest.h>

#include "ecs/column.h"
#include "ecs/table.h"

TEST(Table, AddColumnToTable) {
  auto table = Table();
  auto column1 = Column::create_column<int>();
  column1.push<int>(10);
  column1.push<int>(20);

  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");
  column2.push<std::string>("World");

  table.add_column(0, std::move(column1));
  table.add_column(1, std::move(column2));

  EXPECT_TRUE(table.has_column(0));
  EXPECT_TRUE(table.has_column(1));
  EXPECT_FALSE(table.has_column(2));

  EXPECT_EQ(table.get_data_unchecked<int>(0, 0), 10);
  EXPECT_EQ(table.get_data_unchecked<int>(0, 1), 20);
  EXPECT_EQ(table.get_data_unchecked<std::string>(1, 0), "Hello");
  EXPECT_EQ(table.get_data_unchecked<std::string>(1, 1), "World");
}