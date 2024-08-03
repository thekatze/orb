#include "vulkan_platform.h"

#ifdef ORB_PLATFORM_WINDOWS

#define VK_USE_PLATFORM_WIN32_KHR
#include "../vulkan_types.h"

#include "../../../core/asserts.h"
#include "../../../platform/platform.h"

typedef struct win32_handle_info {
    HINSTANCE hinstance;
    HWND hwnd;
} win32_handle_info;

b8 orb_vulkan_platform_surface_init(orb_vulkan_context *context) {
    usize size;
    win32_handle_info handle;
    orb_platform_get_window_handle_info(&size, &handle);

    ORB_ASSERT(size == sizeof(win32_handle_info),
               "window handle information definitions do not match");

    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = handle.hinstance,
        .hwnd = handle.hwnd,
    };

    ORB_VK_EXPECT(vkCreateWin32SurfaceKHR(context->instance, &create_info, context->allocator,
                                          &context->surface));

    return true;
}

#endif
