#include <gtest/gtest.h>

#include "ecs/column/column.h"
#include "ecs/column/iterator.h"
#include "ecs/primitive.h"

class ColumnIteratorTest : public testing::Test {
 protected:
  struct TestStruct {
   public:
    explicit TestStruct(std::string&& item) : item_{item} {}
    [[nodiscard]] auto item() const -> std::string { return item_; };

   private:
    std::string item_;
  };

  Column column = Column::create_column<TestStruct>();
  ColumnIterator<TestStruct> iterator;

  void SetUp() override {
    column.push<TestStruct>(TestStruct{"Hello"});
    column.push<TestStruct>(TestStruct{" from"});
    column.push<TestStruct>(TestStruct{" the"});
    column.push<TestStruct>(TestStruct{" other"});
    column.push<TestStruct>(TestStruct{" side"});

    iterator = column.begin<TestStruct>();
  }
};

TEST_F(ColumnIteratorTest, Dereference) {
  EXPECT_EQ(iterator->item(), "Hello");
  EXPECT_EQ((*iterator).item(), "Hello");
}

TEST_F(ColumnIteratorTest, Advance) {
  ++iterator;
  EXPECT_EQ(iterator->item(), " from");
}

TEST_F(ColumnIteratorTest, Comparison) {
  // iter from the same column
  auto new_iterator = column.begin<TestStruct>();
  EXPECT_EQ(iterator, new_iterator);
  EXPECT_NE(++iterator, new_iterator);

  // iter from different column
  auto other = Column::create_column<TestStruct>();
  other.push<TestStruct>(TestStruct("Hello"));
  EXPECT_NE(other.begin<TestStruct>(), new_iterator);
}

TEST_F(ColumnIteratorTest, Loop) {
  std::string s;
  for (auto& it : column.iter<TestStruct>()) {
    s += it.item();
  }
  EXPECT_EQ(s, "Hello from the other side");
}
