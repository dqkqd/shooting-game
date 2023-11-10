#include <gtest/gtest.h>

#include "ecs/column.h"

TEST(Column, BasicConstruct) {
  auto int_column = Column::create_column<int>();
  EXPECT_TRUE(int_column.is<int>());
  EXPECT_FALSE(int_column.is<float>());
  EXPECT_FALSE(int_column.is<std::string>());
  EXPECT_FALSE(int_column.is<size_t>());

  EXPECT_EQ(int_column.component_id(), ColumnCounter::id<int>());
  EXPECT_NE(int_column.component_id(), ColumnCounter::id<float>());
  EXPECT_NE(int_column.component_id(), ColumnCounter::id<std::string>());
  EXPECT_NE(int_column.component_id(), ColumnCounter::id<size_t>());
}

TEST(Column, ColumnInvalidAfterMoved) {
  auto column1 = Column::create_column<int>();
  EXPECT_TRUE(column1.is_valid());
  auto column2 = std::move(column1);
  EXPECT_TRUE(column2.is_valid());
  EXPECT_FALSE(column1.is_valid());
}

TEST(Column, AddElement) {
  auto int_column = Column::create_column<int>();

  int_column.push<int>(15);
  int_column.push<int>(45);
  int_column.push<int>(30);
  EXPECT_EQ(int_column.size(), 3);

  EXPECT_EQ(int_column.get_data_unchecked<int>(0), 15);
  EXPECT_EQ(int_column.get_data_unchecked<int>(1), 45);
  EXPECT_EQ(int_column.get_data_unchecked<int>(2), 30);
  EXPECT_EQ(int_column.get_data<int>(0)->get(), 15);  // NOLINT
  EXPECT_EQ(int_column.get_data<int>(1)->get(), 45);  // NOLINT
  EXPECT_EQ(int_column.get_data<int>(2)->get(), 30);  // NOLINT

  // add non integer element
  EXPECT_ANY_THROW(int_column.push<float>(1.5));
  EXPECT_ANY_THROW(int_column.push<std::string>("hello"));
  EXPECT_ANY_THROW(int_column.push<size_t>(1));
}

TEST(Column, AddTooManyElement) {
  auto int_column = Column::create_column<int>();
  for (int i = 0; i < 1000; ++i) {
    int_column.push<int>(static_cast<int>(i));
  }
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(int_column.get_data_unchecked<int>(i), i);
  }
}

TEST(Column, ReassignElement) {
  auto int_column = Column::create_column<int>();
  int_column.push<int>(10);
  int_column.push<int>(20);

  int_column.get_data_unchecked<int>(0) = 30;
  int_column.get_data_unchecked<int>(1) = 40;

  EXPECT_EQ(int_column.get_data_unchecked<int>(0), 30);
  EXPECT_EQ(int_column.get_data_unchecked<int>(1), 40);
}

TEST(Column, MoveConstruct) {
  auto int_column = Column::create_column<int>();
  int_column.push<int>(10);
  int_column.push<int>(20);

  auto moved_op_column = std::move(int_column);
  EXPECT_EQ(moved_op_column.get_data_unchecked<int>(0), 10);
  EXPECT_EQ(moved_op_column.get_data_unchecked<int>(1), 20);
  EXPECT_FALSE(int_column.is_valid());
}

TEST(Column, ColumnForString) {
  auto column = Column::create_column<std::string>();
  column.push<std::string>("Hello");
  column.push_unchecked<std::string>("World");
  EXPECT_EQ(column.get_data_unchecked<std::string>(0), "Hello");
  EXPECT_EQ(column.get_data_unchecked<std::string>(1), "World");
}

TEST(Column, ColumnForStruct) {
  struct TestStruct {
   public:
    explicit TestStruct(std::string word) : word_{std::move(word)} {}
    auto run() -> std::string { return word_; }

   private:
    std::string word_;
  };

  auto column = Column::create_column<TestStruct>();
  column.push<TestStruct>(TestStruct("Hello"));
  column.push_unchecked<TestStruct>(TestStruct("World"));

  EXPECT_EQ(column.get_data_unchecked<TestStruct>(0).run(), "Hello");
  EXPECT_EQ(column.get_data_unchecked<TestStruct>(1).run(), "World");
}

TEST(Column, Iterator) {
  struct TestStruct {
   public:
    explicit TestStruct(std::string &&item) : item_{item} {}
    [[nodiscard]] auto item() const -> std::string { return item_; };

   private:
    std::string item_;
  };

  auto column = Column::create_column<TestStruct>();
  column.push_unchecked<TestStruct>(TestStruct{"Hello"});
  column.push_unchecked<TestStruct>(TestStruct{" from"});
  column.push_unchecked<TestStruct>(TestStruct{" the"});
  column.push_unchecked<TestStruct>(TestStruct{" other"});
  column.push_unchecked<TestStruct>(TestStruct{" side"});

  std::string items;
  for (auto &iter : column.iter<TestStruct>()) {
    items += iter.item();
  }

  EXPECT_EQ(items, "Hello from the other side");
}