#include "vulkan_platform.h"

#ifdef ORB_PLATFORM_LINUX

#define VK_USE_PLATFORM_XCB_KHR
#include "../vulkan_types.h"

#include "../../../core/asserts.h"
#include "../../../platform/platform.h"

typedef struct linux_handle_info {
    xcb_connection_t *connection;
    xcb_window_t window;
} linux_handle_info;

b8 orb_vulkan_platform_surface_init(orb_vulkan_context *context) {
    usize size;
    linux_handle_info handle;
    orb_platform_get_window_handle_info(&size, &handle);

    ORB_ASSERT(size == sizeof(linux_handle_info),
               "window handle information definitions do not match");

    VkXcbSurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .connection = handle.connection,
        .window = handle.window,
    };

    ORB_VK_EXPECT(vkCreateXcbSurfaceKHR(context->instance, &create_info, context->allocator,
                                        &context->surface));

    return true;
}

#endif
