cmake_minimum_required(VERSION 3.5)
project(shoot_yourself)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
option(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG dad1a84be4e9236267dd7c5393b56c60b3ed7875
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )

FetchContent_MakeAvailable(SDL3)

# Create your game executable target as usual
add_executable(shoot_yourself src/main.cpp)

# Link to the actual SDL3 library.
target_link_libraries(shoot_yourself PRIVATE SDL3::SDL3)
