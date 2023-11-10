#include <gtest/gtest.h>

#include "ecs/column.h"
#include "ecs/table.h"

TEST(Table, AddColumnToTable) {
  auto table = Table(1);

  auto column1 = Column::create_column<int>();
  column1.push<int>(10);
  column1.push<int>(20);

  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");
  column2.push<std::string>("World");

  table.add_column(std::move(column1));
  table.add_column(std::move(column2));

  EXPECT_TRUE(table.has_component(ColumnCounter::id<int>()));
  EXPECT_TRUE(table.has_component(ColumnCounter::id<std::string>()));
  EXPECT_FALSE(table.has_component(ColumnCounter::id<float>()));

  EXPECT_EQ(table.get_data_unchecked<int>(ColumnCounter::id<int>(), 0), 10);
  EXPECT_EQ(table.get_data_unchecked<int>(ColumnCounter::id<int>(), 1), 20);
  EXPECT_EQ(table.get_data_unchecked<std::string>(
                ColumnCounter::id<std::string>(), 0),
            "Hello");
  EXPECT_EQ(table.get_data_unchecked<std::string>(
                ColumnCounter::id<std::string>(), 1),
            "World");
}