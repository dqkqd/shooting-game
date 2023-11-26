#include <gtest/gtest.h>

#include "utils.h"

auto main(int argc, char** argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(new SDLEnvironment);  // NOLINT
  return RUN_ALL_TESTS();
}
