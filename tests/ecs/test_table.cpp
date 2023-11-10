#include <gtest/gtest.h>

#include "ecs/column.h"
#include "ecs/table.h"

TEST(Table, ShouldHaveDifferentId) {
  auto table1 = Table::create_table();
  auto table2 = Table::create_table();
  EXPECT_NE(table1.table_id(), table2.table_id());
}

TEST(Table, AddColumn) {
  auto table = Table::create_table();

  auto column1 = Column::create_column<int>();
  column1.push<int>(10);
  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");

  table.add_column(std::move(column1));
  table.add_column(std::move(column2));

  EXPECT_TRUE(table.has_component(ColumnCounter::id<int>()));
  EXPECT_TRUE(table.has_component(ColumnCounter::id<std::string>()));
  EXPECT_FALSE(table.has_component(ColumnCounter::id<float>()));
  EXPECT_TRUE(table.has_component<int>());
  EXPECT_TRUE(table.has_component<std::string>());
  EXPECT_FALSE(table.has_component<float>());
}

TEST(Table, GetColumn) {
  auto table = Table::create_table();

  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");
  column2.push<std::string>("World");

  table.add_column(std::move(column2));

  EXPECT_EQ(table.get_column_unchecked(ColumnCounter::id<std::string>())
                .get_data_unchecked<std::string>(0),
            "Hello");
  EXPECT_EQ(table.get_column_unchecked(ColumnCounter::id<std::string>())
                .get_data_unchecked<std::string>(1),
            "World");
  EXPECT_EQ(
      table.get_column_unchecked<std::string>().get_data_unchecked<std::string>(
          0),
      "Hello");
  EXPECT_EQ(
      table.get_column_unchecked<std::string>().get_data_unchecked<std::string>(
          1),
      "World");
}

TEST(Table, GetData) {
  auto table = Table::create_table();

  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");
  column2.push<std::string>("World");

  table.add_column(std::move(column2));

  EXPECT_EQ(table.get_data_unchecked<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data_unchecked<std::string>(1), "World");
}