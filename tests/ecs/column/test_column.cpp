#include <gtest/gtest.h>

#include "ecs/column/column.h"
#include "ecs/primitive.h"

TEST(Column, BasicConstruct) {
  auto int_column = Column::create_column<int>();
  EXPECT_TRUE(int_column.is<int>());
  EXPECT_FALSE(int_column.is<float>());
  EXPECT_FALSE(int_column.is<size_t>());

  EXPECT_EQ(int_column.component_id(), ColumnCounter::id<int>());
  EXPECT_NE(int_column.component_id(), ColumnCounter::id<float>());
  EXPECT_NE(int_column.component_id(), ColumnCounter::id<size_t>());
}

TEST(Column, TestMove) {
  auto column1 = Column::create_column<int>();
  column1.push<int>(10);
  column1.push<int>(20);
  EXPECT_TRUE(column1.is_valid());
  EXPECT_EQ(column1.component_id(), ColumnCounter::id<int>());
  EXPECT_EQ(column1.size(), 2);
  EXPECT_EQ(column1.get_data<int>(0), 10);
  EXPECT_EQ(column1.get_data<int>(1), 20);

  auto column2 = std::move(column1);

  EXPECT_FALSE(column1.is_valid());
  EXPECT_EQ(column1.component_id(), INVALID_COMPONENT_ID);
  EXPECT_EQ(column1.size(), 0);

  EXPECT_TRUE(column2.is_valid());
  EXPECT_EQ(column2.component_id(), ColumnCounter::id<int>());
  EXPECT_EQ(column2.size(), 2);
  EXPECT_EQ(column2.get_data<int>(0), 10);
  EXPECT_EQ(column2.get_data<int>(1), 20);

  Column column3{std::move(column2)};

  EXPECT_FALSE(column2.is_valid());
  EXPECT_EQ(column2.component_id(), INVALID_COMPONENT_ID);
  EXPECT_EQ(column2.size(), 0);

  EXPECT_TRUE(column3.is_valid());
  EXPECT_EQ(column3.component_id(), ColumnCounter::id<int>());
  EXPECT_EQ(column3.size(), 2);
  EXPECT_EQ(column3.get_data<int>(0), 10);
  EXPECT_EQ(column3.get_data<int>(1), 20);
}

TEST(Column, AddElement) {
  auto int_column = Column::create_column<int>();

  int_column.push<int>(15);
  int_column.push<int>(45);
  int_column.push<int>(30);
  EXPECT_EQ(int_column.size(), 3);

  EXPECT_EQ(int_column.get_data<int>(0), 15);
  EXPECT_EQ(int_column.get_data<int>(1), 45);
  EXPECT_EQ(int_column.get_data<int>(2), 30);

  // add non integer element
  EXPECT_ANY_THROW(int_column.push<float>(1.5));
  EXPECT_ANY_THROW(int_column.push<size_t>(1));
}

TEST(Column, AddTooManyElement) {
  auto int_column = Column::create_column<int>();
  for (int i = 0; i < 1000; ++i) {
    int_column.push<int>(static_cast<int>(i));
  }
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(int_column.get_data<int>(i), i);
  }
}

TEST(Column, PushDataFromOtherColumn) {
  struct Card {
    int id;
  };
  struct Person {
    int age;
    Card card;
    [[nodiscard]] auto get() const -> int { return age + card.id; }
  };

  auto column1 = Column::create_column<Person>();
  {
    auto column2 = Column::create_column<Person>();
    column2.push<Person>(Person{.age = 10, .card = Card{.id = 1}});
    column2.push<Person>(Person{.age = 20, .card = Card{.id = 2}});
    column2.push<Person>(Person{.age = 30, .card = Card{.id = 3}});
    column2.push<Person>(Person{.age = 40, .card = Card{.id = 4}});
    column2.push<Person>(Person{.age = 50, .card = Card{.id = 5}});

    column1.push_from(column2, 4);
    column1.push_from(column2, 3);
    column1.push_from(column2, 2);
    column1.push_from(column2, 1);
    column1.push_from(column2, 0);
  }

  // data still persist after out of scope
  EXPECT_EQ(column1.get_data<Person>(0).get(), 55);
  EXPECT_EQ(column1.get_data<Person>(1).get(), 44);
  EXPECT_EQ(column1.get_data<Person>(2).get(), 33);
  EXPECT_EQ(column1.get_data<Person>(3).get(), 22);
  EXPECT_EQ(column1.get_data<Person>(4).get(), 11);
}

TEST(Column, AddTooManyUnknownElement) {
  auto int_column = Column::create_column<int>();
  for (int i = 0; i < 1000; ++i) {
    int_column.push_unknown(&i);
  }
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(int_column.get_data<int>(i), i);
  }
}

TEST(Column, ReassignElement) {
  auto int_column = Column::create_column<int>();
  int_column.push<int>(10);
  int_column.push<int>(20);

  int_column.get_data<int>(0) = 30;
  int_column.get_data<int>(1) = 40;

  EXPECT_EQ(int_column.get_data<int>(0), 30);
  EXPECT_EQ(int_column.get_data<int>(1), 40);
}

TEST(Column, RemoveElement) {
  auto int_column = Column::create_column<int>();
  int_column.push<int>(10);
  int_column.push<int>(20);
  int_column.push<int>(30);
  int_column.push<int>(40);

  EXPECT_TRUE(int_column.remove(1));
  EXPECT_EQ(int_column.get_data<int>(0), 10);
  EXPECT_EQ(int_column.get_data<int>(1), 40);
  EXPECT_EQ(int_column.get_data<int>(2), 30);
  EXPECT_EQ(int_column.size(), 3);

  EXPECT_FALSE(int_column.remove(3));

  EXPECT_TRUE(int_column.remove(0));
  EXPECT_EQ(int_column.get_data<int>(0), 30);
  EXPECT_EQ(int_column.get_data<int>(1), 40);
  EXPECT_EQ(int_column.size(), 2);

  EXPECT_TRUE(int_column.remove(1));
  EXPECT_EQ(int_column.get_data<int>(0), 30);
  EXPECT_EQ(int_column.size(), 1);
}

TEST(Column, RemoveManyElements) {
  auto int_column = Column::create_column<int>();
  for (int i = 0; i < 1000; ++i) {
    int_column.push<int>(static_cast<int>(i));
  }
  for (int i = 0; i < 900; ++i) {
    EXPECT_TRUE(int_column.remove(88));
  }
  EXPECT_EQ(int_column.size(), 100);
}

TEST(Column, ColumnForStruct) {
  struct TestStruct {
    float item;
    [[nodiscard]] auto triple() const -> float { return item * 3; }
  };

  auto column = Column::create_column<TestStruct>();
  column.push<TestStruct>(TestStruct{.item = 2.0F});
  column.push<TestStruct>(TestStruct{.item = 3.0F});

  EXPECT_EQ(column.get_data<TestStruct>(0).triple(), 6.0F);
  EXPECT_EQ(column.get_data<TestStruct>(1).triple(), 9.0F);
}

TEST(Column, Clone) {
  struct A {};
  auto column = Column::create_column<A>();
  auto cloned = column.clone();

  EXPECT_TRUE(cloned.is_valid());
  EXPECT_TRUE(cloned.is<A>());
  EXPECT_EQ(cloned.size(), 0);
}
