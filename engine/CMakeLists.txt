set(orb_INCLUDE_DIR "${CMAKE_HOME_DIRECTORY}/engine/src/" PARENT_SCOPE)

SET(MAC_PLATFORM_SRC "")
if (APPLE) 
    enable_language(OBJC)
    SET(MAC_PLATFORM_SRC "./src/orb/platform/platform_mac.m")
endif()

add_library(orb 
    ./src/orb/core/logger.c
    ./src/orb/core/application.c
    ./src/orb/core/orb_memory.c
    ./src/orb/core/orb_string.c
    ./src/orb/core/event.c
    ./src/orb/core/input.c
    ./src/orb/core/clock.c

    "${MAC_PLATFORM_SRC}"
    ./src/orb/platform/platform_windows.c
    ./src/orb/platform/platform_linux.c

    ./src/orb/platform/filesystem_posix.c

    ./src/orb/containers/dynamic_array.c

    ./src/orb/allocators/linear_allocator.c

    ./src/orb/math/orb_math.c
    ./src/orb/math/random.c
    
    ./src/orb/renderer/renderer_frontend.c
    ./src/orb/renderer/renderer_backend.c

    ./src/orb/renderer/vulkan/vulkan_backend.c
    ./src/orb/renderer/vulkan/vulkan_device.c
    ./src/orb/renderer/vulkan/vulkan_swapchain.c
    ./src/orb/renderer/vulkan/vulkan_image.c
    ./src/orb/renderer/vulkan/vulkan_renderpass.c
    ./src/orb/renderer/vulkan/vulkan_command_buffer.c
    ./src/orb/renderer/vulkan/vulkan_framebuffer.c
    ./src/orb/renderer/vulkan/vulkan_fence.c
    ./src/orb/renderer/vulkan/vulkan_shader_utils.c

    ./src/orb/renderer/vulkan/shaders/vulkan_object_shader.c

    ./src/orb/renderer/vulkan/platform/vulkan_platform_mac.c
    ./src/orb/renderer/vulkan/platform/vulkan_platform_windows.c
    ./src/orb/renderer/vulkan/platform/vulkan_platform_linux.c
)

target_compile_definitions(orb PUBLIC ORB_EXPORT ORB_ASSERTIONS_ENABLED)

if(ORB_RELEASE)
    target_compile_definitions(orb PUBLIC ORB_RELEASE)
endif()

# vulkan
find_package(Vulkan REQUIRED COMPONENTS glslc)

add_shaders(orb
    ./src/orb/renderer/vulkan/shaders/orb.builtin.object.vert
    ./src/orb/renderer/vulkan/shaders/orb.builtin.object.frag
)

target_include_directories(orb PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(orb PRIVATE Vulkan::Vulkan)

if(LINUX) 
    find_package(X11 REQUIRED)

    target_include_directories(orb PRIVATE "${X11_INCLUDE_DIR}")
    target_link_libraries(orb PRIVATE m "${X11_LIBRARIES}" X11::xcb X11::X11_xcb)
elseif(APPLE)
    target_link_libraries(orb PRIVATE "-framework Foundation -framework AppKit -framework QuartzCore")
endif()
