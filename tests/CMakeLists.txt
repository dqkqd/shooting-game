include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    test
    ecs/archetype/test_components.cpp
    ecs/archetype/test_archetype_edges.cpp
    ecs/archetype/test_archetype.cpp
    ecs/archetype/test_archetypes.cpp
    ecs/test_column.cpp
    ecs/test_counter.cpp
    ecs/test_query.cpp
    ecs/test_table.cpp
    ecs/test_world.cpp
    )

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
    test
    PRIVATE GTest::gtest_main
    PRIVATE GAME_SOURCE
)

include(GoogleTest)
gtest_discover_tests(test)
