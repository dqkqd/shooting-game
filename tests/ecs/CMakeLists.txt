enable_testing()

add_executable(
    test_ecs
    archetype/test_archetype.cpp
    archetype/test_archetypes.cpp
    archetype/test_edges.cpp
    archetype/test_finder.cpp
    column/test_column.cpp
    column/test_iterator.cpp
    observer/test_observer.cpp
    query/test_iterator.cpp
    query/test_query.cpp
    table/test_iterator.cpp
    table/test_table.cpp
    test_component.cpp
    test_counter.cpp
    test_world.cpp
    )

target_include_directories(test_ecs PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
    test_ecs
    PRIVATE GTest::gtest_main
    PRIVATE GAME_SOURCE
)

include(GoogleTest)
gtest_discover_tests(test_ecs)
