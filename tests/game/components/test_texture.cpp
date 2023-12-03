#include <gtest/gtest.h>

#include <cstddef>

#include "../utils.h"
#include "SDL_render.h"
#include "config.h"
#include "services/texture.h"

TEST(TextureManager, Add) {
  auto *renderer = SDLEnvironment::renderer();

  auto *texture = SDL_CreateTexture(renderer, 0, 0, 10, 10);

  // add new texture
  auto *added_texture = TextureManager::add("unique_key", texture);
  EXPECT_TRUE(added_texture != NULL);
  EXPECT_TRUE(added_texture == texture);

  // add duplicated texture
  auto *new_texture = SDL_CreateTexture(renderer, 0, 0, 10, 10);
  auto *duplicated_texture = TextureManager::add("unique_key", new_texture);
  EXPECT_TRUE(duplicated_texture == added_texture);
  EXPECT_TRUE(duplicated_texture != new_texture);
}

TEST(TextureManager, AddFromFile) {
  auto *renderer = SDLEnvironment::renderer();

  auto texture = TextureManager::add_from_file(
      renderer, GameConfig::data().player.image.c_str());
  EXPECT_TRUE(texture.has_value());

  auto invalid_texture =
      TextureManager::add_from_file(renderer, "Invalid file");
  EXPECT_FALSE(invalid_texture.has_value());
}

TEST(TextureManager, Get) {
  auto *renderer = SDLEnvironment::renderer();

  auto texture = TextureManager::add_from_file(
      renderer, GameConfig::data().player.image.c_str());
  EXPECT_TRUE(texture.has_value());

  EXPECT_TRUE(
      TextureManager::get(GameConfig::data().player.image.c_str()).has_value());
  EXPECT_FALSE(TextureManager::get("Invalid key").has_value());
}

TEST(TextureManager, Clear) {
  auto *renderer = SDLEnvironment::renderer();

  TextureManager::add_from_file(renderer,
                                GameConfig::data().player.image.c_str());
  EXPECT_NE(TextureManager::size(), 0);
  TextureManager::clear();
  EXPECT_EQ(TextureManager::size(), 0);
}

TEST(TextureHelper, GetTextureSize) {
  auto *renderer = SDLEnvironment::renderer();

  auto *texture = SDL_CreateTexture(renderer, 0, 0, 10, 20);
  auto size = get_texture_size(texture);
  EXPECT_EQ(size.w, 10);
  EXPECT_EQ(size.h, 20);
}
