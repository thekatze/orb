cmake_minimum_required(VERSION 3.28)

SET(CMAKE_C_STANDARD 23)

SET(CMAKE_C_FLAGS "-ggdb -Werror -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-enum -Wconversion -Wunreachable-code")

# -fsanitize=address doesnt work on my arm mac, try on other pc
if (APPLE)
    SET(CMAKE_C_FLAGS_DEBUG "-fsanitize=leak")
else()
    SET(CMAKE_C_FLAGS_DEBUG "-fsanitize=leak -fsanitize=address")
endif()

SET(CMAKE_C_FLAGS_RELEASE "-O2")

if (APPLE)
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/gcc/bin/gcc-14")
endif()

# for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(
    orb
    VERSION 0.1.0
    LANGUAGES C
)

add_subdirectory(engine)
add_subdirectory(example)

