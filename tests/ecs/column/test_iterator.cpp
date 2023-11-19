#include <gtest/gtest.h>

#include "ecs/column/column.h"
#include "ecs/primitive.h"

TEST(Column, Iterator) {
  struct TestStruct {
   public:
    explicit TestStruct(std::string &&item) : item_{item} {}
    [[nodiscard]] auto item() const -> std::string { return item_; };

   private:
    std::string item_;
  };

  auto column = Column::create_column<TestStruct>();
  column.push<TestStruct>(TestStruct{"Hello"});
  column.push<TestStruct>(TestStruct{" from"});
  column.push<TestStruct>(TestStruct{" the"});
  column.push<TestStruct>(TestStruct{" other"});
  column.push<TestStruct>(TestStruct{" side"});

  std::string items;
  for (auto &iter : column.iter<TestStruct>()) {
    items += iter.item();
  }

  EXPECT_EQ(items, "Hello from the other side");
}
