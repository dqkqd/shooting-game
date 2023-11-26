add_executable(
    test_game
    components/test_texture.cpp
    main.cpp
    )

target_include_directories(test_game PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
    test_game
    PRIVATE GTest::gtest_main
    PRIVATE GAME_SOURCE
)

include(GoogleTest)
gtest_discover_tests(test_game)
