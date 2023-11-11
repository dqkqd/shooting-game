#include <gtest/gtest.h>

#include <stdexcept>

#include "ecs/column.h"
#include "ecs/primitive.h"
#include "ecs/table.h"

TEST(Table, TestMove) {
  auto column = Column::create_column<int>();
  column.push_unchecked<int>(1);
  column.push_unchecked<int>(2);

  auto table1 = Table();
  table1.add_column(std::move(column));

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
  EXPECT_EQ(table.width(), 1);
  table.add_column(std::move(column2));
  EXPECT_EQ(table.width(), 2);

  EXPECT_TRUE(table.has_component_id(ColumnCounter::id<int>()));
  EXPECT_TRUE(table.has_component_id(ColumnCounter::id<std::string>()));
  EXPECT_FALSE(table.has_component_id(ColumnCounter::id<float>()));
}

TEST(Table, GetColumn) {
  auto table = Table::create_table();

  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");
  column2.push<std::string>("World");

  table.add_column(std::move(column2));

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
  auto table = Table::create_table();
  table.add_column(Column::create_column<int>());
  table.add_column(Column::create_column<float>());
  table.add_column(Column::create_column<std::string>());
  std::set<ComponentId> expected{ColumnCounter::id<int>(),
                                 ColumnCounter::id<float>(),
                                 ColumnCounter::id<std::string>()};
  EXPECT_EQ(table.components().size(), 3);
  for (auto component_id : table.components()) {
    EXPECT_TRUE(expected.find(component_id) != expected.end());
  }
};

TEST(Table, GetData) {
  auto table = Table::create_table();

  auto column2 = Column::create_column<std::string>();
  column2.push<std::string>("Hello");
  column2.push<std::string>("World");

  table.add_column(std::move(column2));

  EXPECT_EQ(table.get_data_unchecked<std::string>(0), "Hello");
  EXPECT_EQ(table.get_data_unchecked<std::string>(1), "World");
}

TEST(Table, HasMultipleComponents) {
  auto table = Table();
  struct A1 {};
  struct A2 {};
  struct A3 {};
  struct A4 {};
  table.add_column(Column::create_column<A1>());
  table.add_column(Column::create_column<A2>());
  table.add_column(Column::create_column<A3>());

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

TEST(Table, AddColumnWithDifferentSizeThrowError) {
  auto column1 = Column::create_column<int>();
  column1.push_unchecked<int>(1);
  column1.push_unchecked<int>(2);

  auto column2 = Column::create_column<int>();
  column2.push_unchecked<int>(1);
  auto column3 = Column::create_column<float>();
  column3.push_unchecked<float>(2);

  auto table = Table();
  table.add_column(std::move(column1));

  EXPECT_THROW(table.add_column(std::move(column2)), std::runtime_error);
  EXPECT_THROW(table.add_column(std::move(column3)), std::runtime_error);
}

TEST(Table, AddColumnToEmptyTableShouldIncreaseHeight) {
  auto column = Column::create_column<int>();
  column.push_unchecked<int>(1);
  column.push_unchecked<int>(2);

  auto table = Table();
  EXPECT_TRUE(table.is_empty());
  EXPECT_EQ(table.width(), 0);
  EXPECT_EQ(table.height(), 0);
  table.add_column(std::move(column));

  EXPECT_TRUE(!table.is_empty());
  EXPECT_EQ(table.width(), 1);
  EXPECT_EQ(table.height(), 2);
}

TEST(Table, AddRow) {
  auto column1 = Column::create_column<int>();
  column1.push_unchecked<int>(1);

  auto column2 = Column::create_column<std::string>();
  column2.push_unchecked<std::string>("Hello");

  auto table = Table();
  table.add_column(std::move(column1));
  table.add_column(std::move(column2));

  table.add_row<int, std::string>(2, "World");

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

  auto table = Table();
  table.add_column(Column::create_column<int>());
  table.add_column(Column::create_column<std::string>());
  table.add_column(Column::create_column<A>());

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
