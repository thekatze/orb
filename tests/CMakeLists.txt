add_executable(tests src/main.c)
target_include_directories(tests PRIVATE "${orb_INCLUDE_DIR}")
target_link_libraries(tests PRIVATE orb)

