set(SOURCE_FILES
    ecs/archetype/archetype.cpp
    ecs/archetype/archetypes.cpp
    ecs/column/column.cpp
    ecs/table/table.cpp
    ecs/component.cpp
    components/animation.cpp
    components/physics.cpp
    components/position.cpp
    enemy/rhino.cpp
    player/bullet.cpp
    player/player.cpp
    player/shooter.cpp
    services/texture.cpp
    tiles/tilemap.cpp
    camera.cpp
    game.cpp
    game_state.cpp
    graphic.cpp
    systems.cpp
)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
option(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    )
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    )
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG 6ba90f77752ab0350f28fb228c5dde550244904e
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

FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(SDL3_image)

add_library(GAME_SOURCE ${SOURCE_FILES})

target_include_directories(GAME_SOURCE PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GAME_SOURCE PUBLIC nlohmann_json::nlohmann_json spdlog::spdlog SDL3::SDL3 SDL3_image::SDL3_image)
