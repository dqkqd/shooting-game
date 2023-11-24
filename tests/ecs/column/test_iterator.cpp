#include <gtest/gtest.h>

#include "ecs/column/column.h"
#include "ecs/column/iterator.h"

class ColumnIteratorTest : public testing::Test {
 protected:
  struct TestStruct {
   public:
    float item;
    explicit TestStruct(float float_item) : item{float_item} {}
    [[nodiscard]] auto triple() const -> float { return item * 3; };
  };

  Column column = Column::create_column<TestStruct>();
  ColumnIterator<TestStruct> iterator;

  void SetUp() override {
    column.push<TestStruct>(TestStruct{1.0});
    column.push<TestStruct>(TestStruct{2.0});
    column.push<TestStruct>(TestStruct{3.0});
    column.push<TestStruct>(TestStruct{4.0});
    column.push<TestStruct>(TestStruct{5.0});

    iterator = column.begin<TestStruct>();
  }
};

TEST_F(ColumnIteratorTest, Dereference) {
  EXPECT_EQ(iterator->triple(), 3.0);
  EXPECT_EQ((*iterator).triple(), 3.0);
}

TEST_F(ColumnIteratorTest, Advance) {
  ++iterator;
  EXPECT_EQ(iterator->triple(), 6.0);
}

TEST_F(ColumnIteratorTest, Modify) {
  EXPECT_EQ(iterator->triple(), 3.0);
  (iterator->item) += 10;
  EXPECT_EQ(iterator->triple(), 33.0);
  (*iterator).item += 10;
  EXPECT_EQ(iterator->triple(), 63.0);
}

TEST_F(ColumnIteratorTest, Comparison) {
  // iter from the same column
  auto new_iterator = column.begin<TestStruct>();
  EXPECT_EQ(iterator, new_iterator);
  EXPECT_NE(++iterator, new_iterator);

  // iter from different column
  auto other = Column::create_column<TestStruct>();
  other.push<TestStruct>(TestStruct{1.0});
  EXPECT_NE(other.begin<TestStruct>(), new_iterator);
}

TEST_F(ColumnIteratorTest, Loop) {
  float sum = 0;
  for (auto& it : column.iter<TestStruct>()) {
    sum += it.triple();
  }
  EXPECT_EQ(sum, 45.0);
}
