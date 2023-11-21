#include "ecs/query/query.h"
#include "ecs/world.h"
#include "gtest/gtest.h"

class QueryIteratorTest : public testing::Test {
 protected:
  struct TestStruct {
   public:
    float item;
    explicit TestStruct(float item) : item{item} {}
    [[nodiscard]] auto triple() const -> float { return item * 3; };
  };

  World world = World();
  EntityLocation location1;
  EntityLocation location2;
  EntityLocation location3;
  EntityLocation location4;
  EntityLocation location5;
  EntityLocation location6;

  void SetUp() override {
    location1 =
        world.spawn_entity_with<int, char, TestStruct>(1, 'a', TestStruct{1});
    location2 =
        world.spawn_entity_with<int, char, TestStruct>(2, 'b', TestStruct{2});
    location3 =
        world.spawn_entity_with<int, char, TestStruct>(3, 'c', TestStruct{3});
    location4 = world.spawn_entity_with<int, TestStruct>(4, TestStruct{4});
    location5 = world.spawn_entity_with<TestStruct>(TestStruct{5});
    location6 = world.spawn_entity_with<int>(5);
  }
};

TEST_F(QueryIteratorTest, Get) {
  Query query =
      Query(world.archetypes(),
            world.archetypes().finder().find<int, char, TestStruct>());
  auto iter = query.begin<int, char, TestStruct>();
  auto [i, c, t] = *iter;
  EXPECT_EQ(i, 1);
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(t.triple(), 3);
}

TEST_F(QueryIteratorTest, GetWithMultipleArchetypes) {
  Query query =
      Query(world.archetypes(), world.archetypes().finder().find<TestStruct>());
  auto iter = query.begin<TestStruct>();
  auto [t] = *iter;
  EXPECT_EQ(t.triple(), 3);
}

TEST_F(QueryIteratorTest, Advance) {
  Query query =
      Query(world.archetypes(),
            world.archetypes().finder().find<int, char, TestStruct>());
  auto iter = query.begin<int, char, TestStruct>();
  ++iter;
  auto [i1, c1, t1] = *iter;
  EXPECT_EQ(i1, 2);
  EXPECT_EQ(c1, 'b');
  EXPECT_EQ(t1.triple(), 6);

  iter++;
  auto [i2, c2, t2] = *iter;
  EXPECT_EQ(i2, 3);
  EXPECT_EQ(c2, 'c');
  EXPECT_EQ(t2.triple(), 9);
}

TEST_F(QueryIteratorTest, AdvanceWithMultipleArchetypes) {
  Query query =
      Query(world.archetypes(), world.archetypes().finder().find<TestStruct>());
  auto iter = query.begin<TestStruct>();

  ++iter;
  auto [t1] = *iter;
  EXPECT_EQ(t1.triple(), 6);

  iter++;
  auto [t2] = *iter;
  EXPECT_EQ(t2.triple(), 9);

  ++iter;
  auto [t3] = *iter;
  EXPECT_EQ(t3.triple(), 12);

  iter++;
  auto [t4] = *iter;
  EXPECT_EQ(t4.triple(), 15);
}

TEST_F(QueryIteratorTest, Modify) {
  Query query =
      Query(world.archetypes(),
            world.archetypes().finder().find<int, char, TestStruct>());
  auto iter1 = query.begin<int, char, TestStruct>();
  auto [i1, c1, t1] = *iter1;
  i1 = 10;
  c1 = 'x';
  t1 = TestStruct{10};

  auto iter2 = query.begin<int, char, TestStruct>();
  auto [i2, c2, t2] = *iter1;
  EXPECT_EQ(i2, 10);
  EXPECT_EQ(c2, 'x');
  EXPECT_EQ(t2.triple(), 30);
}

TEST_F(QueryIteratorTest, ModifyWithMultipleArchetypes) {
  Query query =
      Query(world.archetypes(), world.archetypes().finder().find<TestStruct>());
  auto iter1 = query.begin<TestStruct>();
  auto [t1] = *iter1;
  t1 = TestStruct{10};

  auto iter2 = query.begin<TestStruct>();
  auto [t2] = *iter1;
  EXPECT_EQ(t2.triple(), 30);
}

TEST_F(QueryIteratorTest, Done) {
  Query query =
      Query(world.archetypes(),
            world.archetypes().finder().find<int, char, TestStruct>());
  auto iter = query.begin<int, char, TestStruct>();
  ++iter;
  ++iter;
  ++iter;
  EXPECT_TRUE(iter.done());
}

TEST_F(QueryIteratorTest, DoneWithMultipleArchetypes) {
  Query query =
      Query(world.archetypes(), world.archetypes().finder().find<TestStruct>());
  auto iter = query.begin<TestStruct>();
  ++iter;
  ++iter;
  ++iter;
  ++iter;
  ++iter;
  EXPECT_TRUE(iter.done());
}

TEST_F(QueryIteratorTest, Loop) {
  Query query =
      Query(world.archetypes(),
            world.archetypes().finder().find<int, char, TestStruct>());
  std::vector<int> is;
  std::vector<char> cs;
  std::vector<TestStruct> ts;
  for (auto [i, c, t] : query.iter<int, char, TestStruct>()) {
    is.push_back(i);
    cs.push_back(c);
    ts.push_back(t);
  }

  EXPECT_EQ(is, std::vector<int>({1, 2, 3}));
  EXPECT_EQ(cs, std::vector<char>({'a', 'b', 'c'}));
  EXPECT_EQ(ts[0].triple(), 3.0);
  EXPECT_EQ(ts[1].triple(), 6.0);
  EXPECT_EQ(ts[2].triple(), 9.0);
}

TEST_F(QueryIteratorTest, LoopWithMultipleArchetypes) {
  Query query =
      Query(world.archetypes(), world.archetypes().finder().find<TestStruct>());

  std::vector<TestStruct> ts;
  for (auto [t] : query.iter<TestStruct>()) {
    ts.push_back(t);
  }

  EXPECT_EQ(ts[0].triple(), 3.0);
  EXPECT_EQ(ts[1].triple(), 6.0);
  EXPECT_EQ(ts[2].triple(), 9.0);
  EXPECT_EQ(ts[3].triple(), 12.0);
  EXPECT_EQ(ts[4].triple(), 15.0);
}

TEST_F(QueryIteratorTest, NoResultFound) {
  Query query =
      Query(world.archetypes(), world.archetypes().finder().find<double>());

  std::vector<double> ds;
  for (auto [d] : query.iter<double>()) {
    ds.push_back(d);
  }
  EXPECT_TRUE(ds.empty());
}