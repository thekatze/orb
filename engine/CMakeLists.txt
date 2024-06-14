set(orb_INCLUDE_DIR "${CMAKE_HOME_DIRECTORY}/engine/src/" PARENT_SCOPE)

add_library(orb 
    ./src/orb/core/logger.c
    ./src/orb/platform/platform.c 
)

target_compile_definitions(orb PUBLIC ORB_EXPORT ORB_RELEASE=0 ORB_ASSERTIONS_ENABLED)

# target_include_directories(example PRIVATE "${orb_core_INCLUDE_DIR} ${orb_platform_INCLUDE_DIR}")
# target_link_libraries(example PRIVATE orb_platform)

# find_package(Vulkan REQUIRED)
# target_include_directories(orb_render PRIVATE "${Vulkan_INCLUDE_DIR}")
# target_link_libraries(orb_render PRIVATE Vulkan::Vulkan)

