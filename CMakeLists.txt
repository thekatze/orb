cmake_minimum_required(VERSION 3.28)

SET(CMAKE_C_STANDARD 23)

SET(CMAKE_C_FLAGS "-ggdb -Werror -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-enum -Wconversion -Wunreachable-code")

if (APPLE)
    # when debugging platform_mac.m, comment out these lines and remove the -Werror compiler flag.
    # xcode only works with clang that doesnt support c23 yet, so we get extra warnings there
    SET(CMAKE_C_COMPILER "/opt/homebrew/opt/gcc/bin/gcc-14")
    SET(CMAKE_C_FLAGS_DEBUG "-fsanitize=leak")
else()
    # -fsanitize=address does not work on mac
    SET(CMAKE_C_FLAGS_DEBUG "-fsanitize=leak -fsanitize=address")
endif()

SET(CMAKE_C_FLAGS_RELEASE "-O2")

# for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(
    orb
    VERSION 0.1.0
    LANGUAGES C
)

add_subdirectory(engine)
add_subdirectory(example)

