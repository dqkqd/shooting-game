#include <gtest/gtest.h>

#include <stdexcept>

#include "ecs/component.h"
#include "ecs/primitive.h"
#include "ecs/table/table.h"

TEST(Table, Constructor) {
  auto table = Table::create_table<int, float, double, std::string>();
  EXPECT_EQ(table.width(), 4);
  EXPECT_EQ(table.height(), 0);
}

TEST(Table, TestMove) {
  auto table1 = Table::create_table<int, float>();
  table1.add_row<int, float>(1, 3.0);
  table1.add_row<int, float>(2, 4.0);

  auto table2 = std::move(table1);
  EXPECT_TRUE(table2.is_valid());
  EXPECT_FALSE(table2.is_empty());
  EXPECT_EQ(table2.width(), 2);
  EXPECT_EQ(table2.height(), 2);
  EXPECT_EQ(table2.components(), (Components::from_types<int, float>()));

  EXPECT_FALSE(table1.is_valid());
  EXPECT_TRUE(table1.is_empty());
  EXPECT_EQ(table1.width(), 0);
  EXPECT_EQ(table1.height(), 0);
  EXPECT_EQ(table1.components(), Components::from_types());

  Table table3{std::move(table2)};
  EXPECT_TRUE(table3.is_valid());
  EXPECT_FALSE(table3.is_empty());
  EXPECT_EQ(table3.width(), 2);
  EXPECT_EQ(table3.height(), 2);
  EXPECT_EQ(table3.components(), (Components::from_types<int, float>()));

  EXPECT_FALSE(table2.is_valid());
  EXPECT_TRUE(table2.is_empty());
  EXPECT_EQ(table2.width(), 0);
  EXPECT_EQ(table2.height(), 0);
  EXPECT_EQ(table2.components(), Components::from_types());
}

TEST(Table, ShouldHaveDifferentId) {
  auto table1 = Table::create_table<int>();
  auto table2 = Table::create_table<int>();
  EXPECT_NE(table1.table_id(), table2.table_id());
}

TEST(Table, GetAllComponents) {
  auto table = Table::create_table<int, float, std::string>();
  EXPECT_EQ(table.components(),
            (Components::from_types<int, float, std::string>()));
};

TEST(Table, GetData) {
  auto table = Table::create_table<std::string>();
  table.add_row<std::string>("Hello");
  table.add_row<std::string>("World");
  EXPECT_EQ(table.get_data<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data<std::string>(1), "World");
}

TEST(Table, HasMultipleComponents) {
  struct A1 {};
  struct A2 {};
  struct A3 {};
  struct A4 {};
  auto table = Table::create_table<A1, A2, A3>();
  EXPECT_TRUE(table.has_components<A1>());
  EXPECT_TRUE((table.has_components<A1, A2>()));
  EXPECT_TRUE((table.has_components<A1, A2, A3>()));
  EXPECT_FALSE((table.has_components<A1, A2, A3, A4>()));
}

TEST(Table, AddRow) {
  auto table = Table::create_table<int, std::string>();
  EXPECT_EQ((table.add_row<int, std::string>(1, "Hello")), 0);
  EXPECT_EQ((table.add_row<int, std::string>(2, "World")), 1);

  EXPECT_EQ(table.height(), 2);
  EXPECT_EQ(table.get_data<int>(0), 1);
  EXPECT_EQ(table.get_data<int>(1), 2);
  EXPECT_EQ(table.get_data<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data<std::string>(1), "World");

  // mismatch type
  EXPECT_ANY_THROW((table.add_row<int, float>(0, 1.0)));
  // mismatch width
  EXPECT_ANY_THROW((table.add_row<int>(0)));
}

TEST(Table, RemoveRow) {
  struct A {
   public:
    explicit A(std::string&& item) : item_{std::move(item)} {}
    auto run() -> std::string { return item_; }

   private:
    std::string item_;
  };

  auto table = Table::create_table<int, std::string, A>();
  table.add_row<int, std::string, A>(10, "Hello", A("Hello"));
  table.add_row<int, std::string, A>(20, "from", A("from"));
  table.add_row<int, std::string, A>(30, "the", A("the"));
  table.add_row<int, std::string, A>(40, "other", A("other"));
  table.add_row<int, std::string, A>(50, "side", A("side"));

  EXPECT_EQ(table.height(), 5);

  EXPECT_TRUE(table.remove_row(2));
  EXPECT_EQ(table.height(), 4);
  EXPECT_EQ(table.get_data<int>(2), 50);
  EXPECT_EQ(table.get_data<std::string>(2), "side");
  EXPECT_EQ(table.get_data<A>(2).run(), "side");

  // no height mismatch
  EXPECT_TRUE(table.is_valid());
}

TEST(Table, MoveRowToOtherSameComponentTypes) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);
  table.add_row<int, float>(3, 4.0);
  table.add_row<int, float>(5, 6.0);
  table.add_row<int, float>(7, 8.0);

  auto other = Table::create_table<int, float>();
  table.move_row_to_other(1, other);

  // since other receive all the components, they must all be valid
  EXPECT_EQ(other.height(), 1);
  EXPECT_EQ(table.height(), 3);
  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(other.is_valid());

  EXPECT_EQ(other.get_data<int>(0), 3);
  EXPECT_EQ(other.get_data<float>(0), 4.0);

  EXPECT_EQ(table.get_data<int>(0), 1);
  EXPECT_EQ(table.get_data<float>(0), 2.0);
  EXPECT_EQ(table.get_data<int>(1), 7);
  EXPECT_EQ(table.get_data<float>(1), 8.0);
  EXPECT_EQ(table.get_data<int>(2), 5);
  EXPECT_EQ(table.get_data<float>(2), 6.0);
}

TEST(Table, MoveRowToOtherHasFewerComponentTypes) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);
  table.add_row<int, float>(3, 4.0);
  table.add_row<int, float>(5, 6.0);
  table.add_row<int, float>(7, 8.0);

  auto other = Table::create_table<int>();
  table.move_row_to_other(1, other);

  // since other receive enough components, they must all be valid
  EXPECT_EQ(other.height(), 1);
  EXPECT_EQ(table.height(), 3);
  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(other.is_valid());

  EXPECT_EQ(other.get_data<int>(0), 3);

  EXPECT_EQ(table.get_data<int>(0), 1);
  EXPECT_EQ(table.get_data<float>(0), 2.0);
  EXPECT_EQ(table.get_data<int>(1), 7);
  EXPECT_EQ(table.get_data<float>(1), 8.0);
  EXPECT_EQ(table.get_data<int>(2), 5);
  EXPECT_EQ(table.get_data<float>(2), 6.0);
}

TEST(Table, MoveRowToOtherHasDifferentComponentTypes) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);
  table.add_row<int, float>(3, 4.0);
  table.add_row<int, float>(5, 6.0);
  table.add_row<int, float>(7, 8.0);

  auto other = Table::create_table<std::string>();
  table.move_row_to_other(1, other);

  // since other did not receive any component, they must all be valid
  EXPECT_EQ(other.height(), 0);
  EXPECT_EQ(table.height(), 3);
  EXPECT_TRUE(table.is_valid());
  EXPECT_TRUE(other.is_valid());
}

TEST(Table, MoveRowToOtherHasSomeTableComponentTypes) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);
  table.add_row<int, float>(3, 4.0);
  table.add_row<int, float>(5, 6.0);
  table.add_row<int, float>(7, 8.0);

  auto other = Table::create_table<int, std::string>();
  table.move_row_to_other(1, other);

  // since other has some of table's component types, other must not be valid
  EXPECT_FALSE(other.is_valid());
  EXPECT_EQ(table.height(), 3);
  EXPECT_TRUE(table.is_valid());
}

TEST(Table, CloneWith) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);

  auto clone = table.clone_with<std::string>();

  EXPECT_TRUE(clone.is_valid());
  EXPECT_TRUE(clone.is_empty());
  EXPECT_TRUE((clone.has_components<int, float, std::string>()));

  EXPECT_THROW(table.clone_with<int>(), std::runtime_error);
}

TEST(Table, CloneWithout) {
  auto table = Table::create_table<int, float>();
  table.add_row<int, float>(1, 2.0);

  auto clone = table.clone_without<float>();

  EXPECT_TRUE(clone.is_valid());
  EXPECT_TRUE(clone.is_empty());
  EXPECT_TRUE((clone.has_components<int>()));
  EXPECT_FALSE((clone.has_components<float>()));

  EXPECT_THROW(clone.clone_without<int>(), std::runtime_error);
}
