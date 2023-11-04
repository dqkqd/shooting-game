cmake_minimum_required(VERSION 3.27)
project(shoot_yourself)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(src)

add_executable(shoot_yourself src/main.cpp)
target_link_libraries(shoot_yourself PUBLIC GAME_SOURCE)
target_include_directories(
    shoot_yourself PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)