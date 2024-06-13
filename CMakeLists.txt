cmake_minimum_required(VERSION 3.28)

SET(CMAKE_C_STANDARD 23)

SET(CMAKE_C_FLAGS "-ggdb -Werror -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-enum -Wconversion -Wunreachable-code")

# -fsanitize=address doesnt work on my arm mac, try on other pc
SET(CMAKE_C_FLAGS_DEBUG "-fsanitize=leak")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

if (APPLE)
    # i want to try out -fsanitize
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/gcc/bin/gcc-14")
endif()

# for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(
    orb
    VERSION 0.1.0
    LANGUAGES C
)

add_subdirectory("orb_platform")

add_executable(orb main.c)
target_include_directories(orb PRIVATE "${orb_platform_INCLUDE_DIR}")
target_link_libraries(orb PRIVATE orb_platform)

# find_package(Vulkan REQUIRED)
# target_include_directories(orb_render PRIVATE "${Vulkan_INCLUDE_DIR}")
# target_link_libraries(orb_render PRIVATE Vulkan::Vulkan)

