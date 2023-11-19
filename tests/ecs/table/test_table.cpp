#include <gtest/gtest.h>

#include <stdexcept>

#include "ecs/component.h"
#include "ecs/primitive.h"
#include "ecs/table/table.h"

class TableTest : public testing::Test {
 protected:
  Table table;

  void SetUp() override {
    table = Table::create_table<int, float, char, std::string>();
    table.add_row<int, float, char, std::string>(1, 1.0, 'a', "Hello");
    table.add_row<int, float, char, std::string>(2, 2.0, 'b', "from");
    table.add_row<int, float, char, std::string>(3, 3.0, 'c', "the");
    table.add_row<int, float, char, std::string>(4, 4.0, 'd', "other");
    table.add_row<int, float, char, std::string>(5, 5.0, 'e', "side");
  }
};

TEST_F(TableTest, Constructor) {
  EXPECT_EQ(table.width(), 4);
  EXPECT_EQ(table.height(), 5);
}

TEST_F(TableTest, TestMove) {
  auto table1 = std::move(table);
  EXPECT_TRUE(table1.is_valid());
  EXPECT_FALSE(table1.is_empty());
  EXPECT_EQ(table1.width(), 4);
  EXPECT_EQ(table1.height(), 5);
  EXPECT_EQ(table1.components(),
            (Components::from_types<int, float, char, std::string>()));

  EXPECT_FALSE(table.is_valid());
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(table.width(), 0);
  EXPECT_EQ(table.height(), 0);
  EXPECT_EQ(table.components(), Components::from_types());

  Table table2{std::move(table1)};
  EXPECT_TRUE(table2.is_valid());
  EXPECT_FALSE(table2.is_empty());
  EXPECT_EQ(table2.width(), 4);
  EXPECT_EQ(table2.height(), 5);
  EXPECT_EQ(table2.components(),
            (Components::from_types<int, float, char, std::string>()));

  EXPECT_FALSE(table1.is_valid());
  EXPECT_TRUE(table1.is_empty());
  EXPECT_EQ(table1.width(), 0);
  EXPECT_EQ(table1.height(), 0);
  EXPECT_EQ(table1.components(), Components::from_types());
}

TEST_F(TableTest, ShouldHaveDifferentId) {
  auto table1 = Table::create_table<int>();
  auto table2 = Table::create_table<int>();
  EXPECT_NE(table1.table_id(), table2.table_id());
}

TEST_F(TableTest, GetAllComponents) {
  EXPECT_EQ(table.components(),
            (Components::from_types<int, float, char, std::string>()));
};

TEST_F(TableTest, GetData) {
  EXPECT_EQ(table.get_data<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data<std::string>(1), "from");
}

TEST_F(TableTest, GetDataRow) {
  EXPECT_EQ(
      (table.get_data_row<int, float, char, std::string>(0)),
      (std::make_tuple<int, float, char, std::string>(1, 1.0, 'a', "Hello")));
}

TEST_F(TableTest, HasMultipleComponents) {
  EXPECT_TRUE(table.has_components<int>());
  EXPECT_TRUE((table.has_components<float, int>()));
  EXPECT_TRUE((table.has_components<float, int, char>()));
  EXPECT_FALSE((table.has_components<int, double, char, std::string>()));
}

TEST_F(TableTest, AddData) {
  EXPECT_EQ(table.height(), 5);
  table.add_data<int, float, char, std::string>(6, 6.0, 'x', "???");

  EXPECT_EQ(table.height(), 6);
  EXPECT_TRUE(table.is_valid());
  EXPECT_EQ(table.get_data<int>(5), 6);
  EXPECT_EQ(table.get_data<std::string>(5), "???");

  // not enough components, table now has invalid height
  table.add_data<int, float>(7, 7.0);
  EXPECT_FALSE(table.is_valid());

  table.add_data<char, std::string>('y', "!!!");
  // missing components were added
  EXPECT_TRUE(table.is_valid());
  EXPECT_EQ(table.height(), 7);
  EXPECT_EQ(table.get_data<int>(6), 7);
  EXPECT_EQ(table.get_data<std::string>(6), "!!!");
}

TEST_F(TableTest, AddRow) {
  EXPECT_EQ(table.height(), 5);
  table.add_row<int, float, char, std::string>(80, 8.0, 'x', "???");

  EXPECT_EQ(table.height(), 6);
  EXPECT_TRUE(table.is_valid());
  EXPECT_EQ(table.get_data<int>(5), 80);
  EXPECT_EQ(table.get_data<std::string>(5), "???");

  // mismatch type
  EXPECT_ANY_THROW((table.add_row<int, float>(0, 1.0)));
  // mismatch width
  EXPECT_ANY_THROW((table.add_row<int>(0)));
}

TEST_F(TableTest, RemoveRow) {
  EXPECT_EQ(table.height(), 5);

  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(table.remove_row(2));
  EXPECT_EQ(table.height(), 4);

  // row 5 switch with row 2
  EXPECT_EQ(table.get_data<int>(2), 5);
  EXPECT_EQ(table.get_data<std::string>(2), "side");

  // remove invalid row
  EXPECT_FALSE(table.remove_row(10));
  EXPECT_EQ(table.height(), 4);

  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(table.remove_row(0));
  }
  EXPECT_EQ(table.height(), 0);
}

TEST_F(TableTest, MoveRowToOtherSameComponentTypes) {
  auto other = Table::create_table<int, float, char, std::string>();
  table.move_row_to_other(1, other);

  // since other receive all the components, they must all be valid
  EXPECT_EQ(other.height(), 1);
  EXPECT_EQ(table.height(), 4);
  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(other.is_valid());

  // other's row 1 = table's row 2 (old)
  EXPECT_EQ(other.get_data<int>(0), 2);

  EXPECT_EQ(table.get_data<int>(0), 1);
  EXPECT_EQ(table.get_data<int>(1), 5);
  EXPECT_EQ(table.get_data<int>(2), 3);
  EXPECT_EQ(table.get_data<int>(3), 4);
}

TEST_F(TableTest, MoveRowToOtherHasFewerComponentTypes) {
  auto other = Table::create_table<int, float, char>();
  table.move_row_to_other(1, other);

  // since other receive enough components, they must all be valid
  EXPECT_EQ(other.height(), 1);
  EXPECT_EQ(table.height(), 4);
  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(other.is_valid());

  EXPECT_EQ(other.get_data<int>(0), 2);
  EXPECT_EQ(other.get_data<float>(0), 2.0);
  EXPECT_EQ(other.get_data<char>(0), 'b');
}

TEST_F(TableTest, MoveRowToOtherHasDifferentComponentTypes) {
  auto other = Table::create_table<double, size_t>();
  table.move_row_to_other(1, other);

  // since other did not receive any component, they must all be valid
  EXPECT_EQ(other.height(), 0);
  EXPECT_EQ(table.height(), 4);
  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(other.is_valid());
}

TEST_F(TableTest, MoveRowToOtherHasSomeTableComponentTypes) {
  auto other = Table::create_table<int, std::string, double>();
  table.move_row_to_other(1, other);

  // since other has some of table's component types, other must be invalid
  EXPECT_FALSE(other.is_valid());
  EXPECT_EQ(table.height(), 4);
  EXPECT_TRUE(table.is_valid());

  // add double to other should make it valid
  other.add_data<double>(2.0);
  EXPECT_TRUE(other.is_valid());
}

TEST_F(TableTest, CloneWith) {
  auto clone = table.clone_with<double>();

  // cloned should be emptied
  EXPECT_TRUE(clone.is_valid());
  EXPECT_TRUE(clone.is_empty());
  EXPECT_TRUE((clone.has_components<int, float, char, std::string, double>()));

  EXPECT_THROW(table.clone_with<int>(), std::runtime_error);
}

TEST_F(TableTest, CloneWithout) {
  auto clone = table.clone_without<float>();

  // cloned should be emptied
  EXPECT_TRUE(clone.is_valid());
  EXPECT_TRUE(clone.is_empty());
  EXPECT_TRUE((clone.has_components<int>()));
  EXPECT_FALSE((clone.has_components<float>()));

  EXPECT_THROW(clone.clone_without<double>(), std::runtime_error);
}
