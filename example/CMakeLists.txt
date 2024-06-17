add_executable(example src/entry.c src/game.c)
target_include_directories(example PRIVATE "${orb_INCLUDE_DIR}")
target_link_libraries(example PRIVATE orb)

