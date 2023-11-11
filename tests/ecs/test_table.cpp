#include <gtest/gtest.h>

#include <stdexcept>

#include "ecs/primitive.h"
#include "ecs/table.h"

TEST(Table, Constructor) {
  auto table = Table::create_table<int, float, double, std::string>();
  EXPECT_EQ(table.width(), 4);
  EXPECT_EQ(table.height(), 0);
}

TEST(Table, TestMove) {
  auto table1 = Table::create_table<int>();
  table1.add_row<int>(1);
  table1.add_row<int>(2);

  auto table2 = std::move(table1);
  EXPECT_TRUE(table2.is_valid());
  EXPECT_FALSE(table2.is_empty());
  EXPECT_EQ(table2.width(), 1);
  EXPECT_EQ(table2.height(), 2);

  EXPECT_FALSE(table1.is_valid());
  EXPECT_TRUE(table1.is_empty());
  EXPECT_EQ(table1.width(), 0);
  EXPECT_EQ(table1.height(), 0);
}

TEST(Table, ShouldHaveDifferentId) {
  auto table1 = Table::create_table<int>();
  auto table2 = Table::create_table<int>();
  EXPECT_NE(table1.table_id(), table2.table_id());
}

TEST(Table, GetColumn) {
  auto table = Table::create_table<std::string>();
  table.add_row<std::string>("Hello");
  table.add_row<std::string>("World");

  EXPECT_EQ(table.get_column(ColumnCounter::id<std::string>())
                .get_data_unchecked<std::string>(0),
            "Hello");
  EXPECT_EQ(table.get_column(ColumnCounter::id<std::string>())
                .get_data_unchecked<std::string>(1),
            "World");
  EXPECT_EQ(table.get_column<std::string>().get_data_unchecked<std::string>(0),
            "Hello");
  EXPECT_EQ(table.get_column<std::string>().get_data_unchecked<std::string>(1),
            "World");
}

TEST(Table, GetAllComponents) {
  auto table = Table::create_table<int, float, std::string>();
  std::set<ComponentId> expected{ColumnCounter::id<int>(),
                                 ColumnCounter::id<float>(),
                                 ColumnCounter::id<std::string>()};
  EXPECT_EQ(table.components().size(), 3);
  for (auto component_id : table.components()) {
    EXPECT_TRUE(expected.find(component_id) != expected.end());
  }
};

TEST(Table, GetData) {
  auto table = Table::create_table<std::string>();
  table.add_row<std::string>("Hello");
  table.add_row<std::string>("World");
  EXPECT_EQ(table.get_data_unchecked<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data_unchecked<std::string>(1), "World");
}

TEST(Table, HasMultipleComponents) {
  struct A1 {};
  struct A2 {};
  struct A3 {};
  struct A4 {};
  auto table = Table::create_table<A1, A2, A3>();
  auto id1 = ColumnCounter::id<A1>();
  auto id2 = ColumnCounter::id<A2>();
  auto id3 = ColumnCounter::id<A3>();
  auto id4 = ColumnCounter::id<A4>();

  EXPECT_TRUE(table.has_components(id1));
  EXPECT_TRUE(table.has_components(id1, id2));
  EXPECT_TRUE(table.has_components(id1, id2, id3));
  EXPECT_FALSE(table.has_components(id1, id2, id3, id4));

  EXPECT_TRUE(table.has_components<A1>());
  // gtest doesn't understand this kind of template function
  auto has_a1_a2 = table.has_components<A1, A2>();
  EXPECT_TRUE(has_a1_a2);
  auto has_a1_a2_a3 = table.has_components<A1, A2, A3>();
  EXPECT_TRUE(has_a1_a2_a3);
  auto has_a1_a2_a3_a4 = table.has_components<A1, A2, A3, A4>();
  EXPECT_FALSE(has_a1_a2_a3_a4);
}

TEST(Table, AddRow) {
  auto table = Table::create_table<int, std::string>();
  EXPECT_EQ((table.add_row<int, std::string>(1, "Hello")), 0);
  EXPECT_EQ((table.add_row<int, std::string>(2, "World")), 1);

  EXPECT_EQ(table.height(), 2);
  EXPECT_EQ(table.get_data_unchecked<int>(0), 1);
  EXPECT_EQ(table.get_data_unchecked<int>(1), 2);
  EXPECT_EQ(table.get_data_unchecked<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data_unchecked<std::string>(1), "World");

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
  EXPECT_EQ(table.get_data_unchecked<int>(2), 50);
  EXPECT_EQ(table.get_data_unchecked<std::string>(2), "side");
  EXPECT_EQ(table.get_data_unchecked<A>(2).run(), "side");
}
