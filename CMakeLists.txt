cmake_minimum_required(VERSION 3.28)

SET(CMAKE_C_STANDARD 23)

if (APPLE)
    # when debugging platform_mac.m, comment out these lines and remove the -Werror compiler flag.
    # xcode only works with clang that doesnt support c23 yet, so we get extra warnings there
    SET(CMAKE_C_COMPILER "/usr/local/bin/clang-19")
    SET(DEBUG_C_FLAGS "-ggdb")
elseif(LINUX)
    # -fsanitize=address does not work on mac
    SET(DEBUG_C_FLAGS "-g3") # -fsanitize=leak -fsanitize=address")
else()
    # -fsanitize=leak does not work on windows
    SET(DEBUG_C_FLAGS "-ggdb -fsanitize=address")
endif()

SET(RELEASE_C_FLAGS)

if (CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building orb in release mode")
    SET(ORB_RELEASE TRUE)
    SET(RELEASE_C_FLAGS "-O2")
else()
    message("Building orb in debug mode")
    SET(ORB_RELEASE FALSE)
endif()

SET(CMAKE_C_FLAGS "${DEBUG_C_FLAGS} ${RELEASE_C_FLAGS} -Werror -Wall -Wpedantic -Wno-gnu-zero-variadic-macro-arguments -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch -Wconversion -Wunreachable-code")

# for clangd
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(
    orb
    VERSION 0.1.0
    LANGUAGES C
)

include("./cmake/add_shaders.cmake")

add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tests)

