add_executable(example main.c platform/platform.c)

# target_include_directories(example PRIVATE "${orb_core_INCLUDE_DIR} ${orb_platform_INCLUDE_DIR}")
# target_link_libraries(example PRIVATE orb_platform)

# find_package(Vulkan REQUIRED)
# target_include_directories(orb_render PRIVATE "${Vulkan_INCLUDE_DIR}")
# target_link_libraries(orb_render PRIVATE Vulkan::Vulkan)

