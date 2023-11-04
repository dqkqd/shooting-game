set(SOURCE_FILES
    game.cpp
    graphic.cpp
)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
option(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main 
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    )
FetchContent_MakeAvailable(SDL3)

add_library(GAME_SOURCE ${SOURCE_FILES})

target_include_directories(GAME_SOURCE PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GAME_SOURCE PUBLIC SDL3::SDL3)