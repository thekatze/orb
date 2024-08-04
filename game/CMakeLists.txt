add_executable(game src/entry.c src/game.c)
target_include_directories(game PRIVATE "${orb_INCLUDE_DIR}")
target_link_libraries(game PRIVATE orb)

# release optimizations
if (ORB_RELEASE) 
    add_custom_command(
        TARGET game 
        POST_BUILD
        COMMAND ${CMAKE_STRIP} "$<TARGET_FILE:game>")

    # could not get lto working on apple silicon
    if (NOT APPLE)
        set_property(
            TARGET game 
            PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()
