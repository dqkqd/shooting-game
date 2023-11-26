#include <gtest/gtest.h>

#include "../utils.h"
#include "components/texture.h"

TEST(TextureManager, AddTextureFromFile) {
  auto *renderer = SDLEnvironment::renderer();

  auto texture =
      TextureManager::add_from_file(renderer, texture_file::TOP_MIDDLE_GRASS);
  EXPECT_TRUE(texture.has_value());

  auto invalid_texture =
      TextureManager::add_from_file(renderer, "Invalid file");
  EXPECT_FALSE(invalid_texture.has_value());
}
