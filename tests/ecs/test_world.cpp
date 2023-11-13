#include "ecs/world.h"
#include "gtest/gtest.h"

TEST(World, SpawnEntityWith) {
  struct A {
   public:
    explicit A(std::string&& item) : item_{std::move(item)} {}
    auto run() -> std::string { return item_; }

   private:
    std::string item_;
  };

  auto world = World();
  auto entity_location =
      world.spawn_entity_with<A, int, std::string>(A{"Hello"}, 1, "World");
}
