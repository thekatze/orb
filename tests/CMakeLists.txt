add_executable(tests 
    ./src/main.c 
    ./src/test_runner.c 

    ./src/allocators/linear_allocator_tests.c
)

target_include_directories(tests PRIVATE "${orb_INCLUDE_DIR}")
target_link_libraries(tests PRIVATE orb)

