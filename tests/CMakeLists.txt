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
    ecs/archetype/test_archetype.cpp
    ecs/archetype/test_archetypes.cpp
    ecs/archetype/test_edges.cpp
    ecs/archetype/test_finder.cpp
    ecs/column/test_column.cpp
    ecs/column/test_iterator.cpp
    ecs/observer/test_observer.cpp
    ecs/query/test_iterator.cpp
    ecs/query/test_query.cpp
    ecs/table/test_iterator.cpp
    ecs/table/test_table.cpp
    ecs/test_component.cpp
    ecs/test_counter.cpp
    ecs/test_world.cpp
    components/test_texture.cpp
    main.cpp
    )

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
    test
    PRIVATE GTest::gtest_main
    PRIVATE GAME_SOURCE
)

include(GoogleTest)
gtest_discover_tests(test)
