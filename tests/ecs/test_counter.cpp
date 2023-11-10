#include <gtest/gtest.h>

#include <thread>

#include "ecs/counter.h"

TEST(Counter, CounterShouldEqualEveryCall) {
  using IntCounter = Counter<int>;

  auto counter_id = IntCounter::id<int>();
  for (int i = 0; i < 20; ++i) {
    EXPECT_EQ(counter_id, IntCounter::id<int>());
  }
}

TEST(Counter, CounterShouldDifferentBetweenTypes) {
  using IntCounter = Counter<int>;
  auto int_value = IntCounter::id<int>();
  auto float_value = IntCounter::id<float>();
  EXPECT_NE(int_value, float_value);
}

TEST(Counter, CounterShouldEqualBetweenTemplates) {
  using IntCounter = Counter<int>;
  using FloatCounter = Counter<float>;

  EXPECT_EQ(IntCounter::id<int>(), 0);
  EXPECT_EQ(IntCounter::id<float>(), 1);
  EXPECT_EQ(IntCounter::id<char>(), 2);
  EXPECT_EQ(IntCounter::id<std::string>(), 3);
  EXPECT_EQ(IntCounter::id<int>(), 0);

  EXPECT_EQ(FloatCounter::id<int>(), 0);
  EXPECT_EQ(FloatCounter::id<float>(), 1);
  EXPECT_EQ(FloatCounter::id<char>(), 2);
  EXPECT_EQ(FloatCounter::id<std::string>(), 3);
  EXPECT_EQ(FloatCounter::id<int>(), 0);
}

TEST(Counter, CounterShouldBeThreadSafe) {
  struct A1 {};
  struct A2 {};
  struct A3 {};
  struct A4 {};
  struct A5 {};
  struct A6 {};
  struct A7 {};
  struct A8 {};
  struct A9 {};
  struct A10 {};
  struct A11 {};
  struct A12 {};

  using IntCounter = Counter<std::size_t>;
  {
    std::vector<std::jthread> pool;
    pool.emplace_back([]() { IntCounter::id<A1>(); });
    pool.emplace_back([]() { IntCounter::id<A2>(); });
    pool.emplace_back([]() { IntCounter::id<A3>(); });
    pool.emplace_back([]() { IntCounter::id<A4>(); });
    pool.emplace_back([]() { IntCounter::id<A5>(); });
    pool.emplace_back([]() { IntCounter::id<A6>(); });
    pool.emplace_back([]() { IntCounter::id<A7>(); });
    pool.emplace_back([]() { IntCounter::id<A8>(); });
    pool.emplace_back([]() { IntCounter::id<A9>(); });
    pool.emplace_back([]() { IntCounter::id<A10>(); });
  }

  EXPECT_EQ(IntCounter::id<A11>(), 10);
  EXPECT_EQ(IntCounter::id<A12>(), 11);
}

TEST(InstanceCounter, IdDifferentBetweenInstance) {
  struct A {};
  using ACounter = InstanceCounter<int>;
  auto instance1 = ACounter::id();
  auto instance2 = ACounter::id();
  EXPECT_NE(instance1, instance2);
}

TEST(InstanceCounter, IdSameBetweenType) {
  struct A {};
  struct B {};
  using ACounter = InstanceCounter<A>;
  using BCounter = InstanceCounter<B>;

  auto a_instance1 = ACounter::id();
  auto a_instance2 = ACounter::id();
  auto b_instance1 = BCounter::id();
  auto b_instance2 = BCounter::id();

  EXPECT_EQ(a_instance1, b_instance1);
  EXPECT_EQ(a_instance2, b_instance2);
}

TEST(InstanceCounter, ShouldBeThreadSafe) {
  struct A {};

  using ACounter = InstanceCounter<A>;
  {
    std::vector<std::jthread> pool;
    pool.reserve(10);
    for (int i = 0; i < 10; ++i) {
      pool.emplace_back([]() {
        for (int j = 0; j < 1000; ++j) {
          ACounter::id();
        }
      });
    }
  }

  EXPECT_EQ(ACounter::id(), 10 * 1000);
  EXPECT_EQ(ACounter::id(), 10 * 1000 + 1);
}