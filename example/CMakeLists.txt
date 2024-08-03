add_executable(example src/entry.c src/game.c)
target_include_directories(example PRIVATE "${orb_INCLUDE_DIR}")
target_link_libraries(example PRIVATE orb)

# release optimizations
if (ORB_RELEASE) 
    add_custom_command(
        TARGET example 
        POST_BUILD
        COMMAND ${CMAKE_STRIP} "$<TARGET_FILE:example>")

    # could not get lto working on apple silicon
    if (NOT APPLE)
        set_property(
            TARGET example 
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()
