cmake_minimum_required(VERSION 3.18)
project(shooting_game)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wstrict-overflow=5")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY config.json DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(src)

add_executable(shooting_game src/main.cpp)
target_link_libraries(shooting_game PUBLIC GAME_SOURCE)
target_include_directories(
    shooting_game PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)

add_subdirectory(tests)
