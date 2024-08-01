#include "vulkan_platform.h"

#ifdef ORB_PLATFORM_MAC

#define VK_USE_PLATFORM_METAL_EXT
#include "../vulkan_types.h"

#include "../../../core/asserts.h"
#include "../../../platform/platform.h"

typedef struct macos_handle_info {
    CAMetalLayer *layer;
} macos_handle_info;

b8 orb_vulkan_platform_surface_init(orb_vulkan_context *context) {
    usize size;
    macos_handle_info handle;
    orb_platform_get_window_handle_info(&size, &handle);

    ORB_ASSERT(size == sizeof(macos_handle_info),
               "window handle information definitions do not match");

    VkMetalSurfaceCreateInfoEXT create_info = {
        .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
        .pLayer = handle.layer,
    };

    ORB_VK_EXPECT(vkCreateMetalSurfaceEXT(context->instance, &create_info, context->allocator,
                                          &context->surface));

    return true;
}

#endif
