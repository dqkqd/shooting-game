set(SOURCE_FILES
    game.cpp
    graphic.cpp
    entity.cpp
    components/graphic.cpp
    ecs/archetype/archetype_components.cpp
    ecs/archetype/archetype_edges.cpp
    ecs/archetype/archetype.cpp
    ecs/archetype/archetypes.cpp
    ecs/column.cpp
    ecs/table.cpp
    ecs/world.cpp
    error.cpp
)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
option(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    )
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    )
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    )

FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(SDL3_image)

add_library(GAME_SOURCE ${SOURCE_FILES})

target_include_directories(GAME_SOURCE PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GAME_SOURCE PUBLIC fmt::fmt SDL3::SDL3 SDL3_image::SDL3_image)
