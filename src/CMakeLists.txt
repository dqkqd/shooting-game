set(SOURCE_FILES
    game.cpp
)

add_library(GAME_SOURCE ${SOURCE_FILES})

target_include_directories(GAME_SOURCE PRIVATE ${PROJECT_SOURCE_DIR}/include)