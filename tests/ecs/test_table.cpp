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

  EXPECT_EQ(other.get_data_unchecked<int>(0), 3);
  EXPECT_EQ(other.get_data_unchecked<float>(0), 4.0);

  EXPECT_EQ(table.get_data_unchecked<int>(0), 1);
  EXPECT_EQ(table.get_data_unchecked<float>(0), 2.0);
  EXPECT_EQ(table.get_data_unchecked<int>(1), 7);
  EXPECT_EQ(table.get_data_unchecked<float>(1), 8.0);
  EXPECT_EQ(table.get_data_unchecked<int>(2), 5);
  EXPECT_EQ(table.get_data_unchecked<float>(2), 6.0);
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

  EXPECT_EQ(other.get_data_unchecked<int>(0), 3);

  EXPECT_EQ(table.get_data_unchecked<int>(0), 1);
  EXPECT_EQ(table.get_data_unchecked<float>(0), 2.0);
  EXPECT_EQ(table.get_data_unchecked<int>(1), 7);
  EXPECT_EQ(table.get_data_unchecked<float>(1), 8.0);
  EXPECT_EQ(table.get_data_unchecked<int>(2), 5);
  EXPECT_EQ(table.get_data_unchecked<float>(2), 6.0);
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

TEST(Table, Iterator) {
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
