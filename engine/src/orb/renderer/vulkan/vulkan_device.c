#include "vulkan_device.h"
#include "../../core/asserts.h"
#include "../../core/orb_memory.h"
#include "../../core/orb_string.h"
#include "vulkan_types.h"

// TODO: make this configurable
const char *device_required_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef ORB_PLATFORM_MAC
                                            "VK_KHR_portability_subset"
#endif
};
VkPhysicalDeviceFeatures device_requested_features = {
    .samplerAnisotropy = VK_TRUE,
};

b8 select_physical_device(orb_vulkan_context *context);

// a value of zero means unsuitable for use in our engine.
// out_swapchain_support_info.formats and
// out_swapchain_support_info.present_modes must be freed if physical device is
// suitable (non-zero return value).
u8 rank_physical_device(VkPhysicalDevice device, VkSurfaceKHR surface,
                        const VkPhysicalDeviceProperties *properties,
                        const VkPhysicalDeviceFeatures *features,
                        orb_vulkan_physical_device_queue_family_info *out_queue_info,
                        orb_vulkan_swapchain_support_info *out_swapchain_support_info);

b8 orb_vulkan_device_init(orb_vulkan_context *context) {
    if (!select_physical_device(context)) {
        return false;
    }

    ORB_DEBUG("Creating logical device");

    const u32 MAX_QUEUES = 3;
    u32 queue_indices[MAX_QUEUES];
    VkDeviceQueueCreateInfo queue_create_infos[MAX_QUEUES];

    u32 used_queues = 1;

    queue_indices[0] = context->device.queue_info.graphics_family_index;

    if (context->device.queue_info.graphics_family_index !=
        context->device.queue_info.present_family_index) {
        queue_indices[used_queues++] = context->device.queue_info.present_family_index;
    }

    if (context->device.queue_info.graphics_family_index !=
        context->device.queue_info.transfer_family_index) {
        queue_indices[used_queues++] = context->device.queue_info.present_family_index;
    }

    for (u32 i = 0; i < used_queues; ++i) {
        f32 queue_priority = 1.0f;
        u32 queue_count = queue_indices[i] == context->device.queue_info.graphics_family_index
                              ? ORB_MIN(context->device.queue_info.graphics_family_queue_count, 2)
                              : 1;

        VkDeviceQueueCreateInfo queue = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_indices[i],
            .pQueuePriorities = &queue_priority,
            .queueCount = queue_count,
        };

        queue_create_infos[i] = queue;
    }

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = used_queues,
        .pQueueCreateInfos = queue_create_infos,
        .pEnabledFeatures = &device_requested_features,
        .enabledExtensionCount = ORB_ARRAY_LENGTH(device_required_extensions),
        .ppEnabledExtensionNames = device_required_extensions,
    };

    ORB_VK_EXPECT(vkCreateDevice(context->device.physical_device, &device_create_info,
                                 context->allocator, &context->device.logical_device));

    ORB_DEBUG("Obtaining device queues");

    vkGetDeviceQueue(context->device.logical_device,
                     context->device.queue_info.graphics_family_index, 0,
                     &context->device.graphics_queue);
    vkGetDeviceQueue(context->device.logical_device,
                     context->device.queue_info.compute_family_index, 0,
                     &context->device.compute_queue);
    vkGetDeviceQueue(context->device.logical_device,
                     context->device.queue_info.transfer_family_index, 0,
                     &context->device.transfer_queue);
    vkGetDeviceQueue(context->device.logical_device,
                     context->device.queue_info.present_family_index, 0,
                     &context->device.present_queue);

    ORB_DEBUG("Creating command queue pool");
    VkCommandPoolCreateInfo command_pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = context->device.queue_info.graphics_family_index,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };

    ORB_VK_EXPECT(vkCreateCommandPool(context->device.logical_device, &command_pool_info,
                                      context->allocator, &context->device.graphics_command_pool));

    return true;
}

void orb_vulkan_device_shutdown(orb_vulkan_context *context) {
    orb_vulkan_device *device = &context->device;

    if (context->graphics_command_buffers) {
        orb_free(context->graphics_command_buffers,
                 sizeof(orb_vulkan_command_buffer) * context->swapchain.image_count,
                 MEMORY_TAG_RENDERER);
    }

    if (device->graphics_command_pool) {
        // destroying the command pool will implicitly clean up the command buffers
        vkDestroyCommandPool(device->logical_device, device->graphics_command_pool,
                             context->allocator);
    }

    if (device->logical_device) {
        vkDestroyDevice(device->logical_device, context->allocator);
    }

    if (device->swapchain.formats) {
        orb_free(device->swapchain.formats,
                 device->swapchain.format_count * sizeof(VkSurfaceFormatKHR), MEMORY_TAG_RENDERER);
    }
    if (device->swapchain.present_modes) {
        orb_free(device->swapchain.present_modes,
                 device->swapchain.present_mode_count * sizeof(VkPresentModeKHR),
                 MEMORY_TAG_RENDERER);
    }
}

typedef struct orb_ranked_device {
    u8 suitability;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory;
    orb_vulkan_physical_device_queue_family_info queue_info;
    orb_vulkan_swapchain_support_info swapchain_info;
} orb_ranked_device;

b8 select_physical_device(orb_vulkan_context *context) {
    u32 physical_device_count = 0;
    ORB_VK_EXPECT(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, nullptr));

    if (physical_device_count == 0) {
        ORB_FATAL("No Vulkan capable devices found.");
        return false;
    }

    VkPhysicalDevice physical_devices[physical_device_count];
    ORB_VK_EXPECT(
        vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));

    u32 ranked_devices_count = 0;
    orb_ranked_device ranked_devices[physical_device_count];

    for (usize i = 0; i < physical_device_count; ++i) {
        VkPhysicalDevice device = physical_devices[i];
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(device, &memory);

        orb_vulkan_physical_device_queue_family_info queue_info = {0};
        orb_vulkan_swapchain_support_info swapchain_info = {0};

        u8 suitability = rank_physical_device(device, context->surface, &properties, &features,
                                              &queue_info, &swapchain_info);

        if (suitability) {
            orb_ranked_device ranked_device = {
                .device = device,
                .suitability = suitability,
                .properties = properties,
                .memory = memory,
                .swapchain_info = swapchain_info,
                .queue_info = queue_info,
            };

            ranked_devices[ranked_devices_count] = ranked_device;
            ranked_devices_count += 1;
        }
    }

    if (ranked_devices_count == 0) {
        ORB_FATAL("No device fulfilling the minimum requirements of the engine found.");
        return false;
    }

    u8 highest_suitability = 0;
    u32 highest_suitability_device_index = 0;
    for (u32 i = 0; i < ranked_devices_count; i++) {
        orb_ranked_device *device = &ranked_devices[i];
        if (device->suitability > highest_suitability) {
            highest_suitability = device->suitability;
            highest_suitability_device_index = i;
        }
    }

    for (usize i = 0; i < ranked_devices_count; i++) {
        orb_ranked_device *device = &ranked_devices[i];
        if (i != highest_suitability_device_index) {
            orb_free(device->swapchain_info.formats,
                     device->swapchain_info.format_count * sizeof(VkSurfaceFormatKHR),
                     MEMORY_TAG_RENDERER);
            orb_free(device->swapchain_info.present_modes,
                     device->swapchain_info.present_mode_count * sizeof(VkPresentModeKHR),
                     MEMORY_TAG_RENDERER);

            continue;
        }

        ORB_INFO("Using render device %s (GPU Driver %d.%d.%d | Vulkan "
                 "API %d.%d.%d]",
                 device->properties.deviceName, VK_VERSION_MAJOR(device->properties.driverVersion),
                 VK_VERSION_MINOR(device->properties.driverVersion),
                 VK_VERSION_PATCH(device->properties.driverVersion),
                 VK_VERSION_MAJOR(device->properties.apiVersion),
                 VK_VERSION_MINOR(device->properties.apiVersion),
                 VK_VERSION_PATCH(device->properties.apiVersion));

        for (u32 j = 0; j < device->memory.memoryHeapCount; ++j) {
            VkMemoryHeap *heap = &device->memory.memoryHeaps[j];
            f32 available_memory_gib = ((f32)heap->size) / 1024.0f / 1024.0f / 1024.0f;

            if (heap->flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                ORB_INFO("Dedicated GPU Memory: %.2f GiB", available_memory_gib);
            } else {
                ORB_INFO("Shared system memory: %.2f GiB", available_memory_gib);
            }
        }

        context->device.physical_device = device->device;
        context->device.properties = device->properties;
        context->device.memory = device->memory;
        context->device.queue_info = device->queue_info;
        context->device.swapchain = device->swapchain_info;
    }

    return true;
}

b8 orb_vulkan_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                                             orb_vulkan_swapchain_support_info *out_support_info) {
    ORB_VK_EXPECT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
                                                            &out_support_info->capabilities));

    u32 previous_format_count = out_support_info->format_count;
    ORB_VK_EXPECT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface,
                                                       &out_support_info->format_count, nullptr));

    if (out_support_info->format_count == 0) {
        return false;
    }

    if (!out_support_info->formats) {
        out_support_info->formats = (VkSurfaceFormatKHR *)orb_allocate(
            out_support_info->format_count * sizeof(VkSurfaceFormatKHR), MEMORY_TAG_RENDERER);
    } else {
        ORB_DEBUG_ASSERT(out_support_info->format_count == previous_format_count,
                         "amount of supported image formats must not change");
    }

    ORB_VK_EXPECT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, surface, &out_support_info->format_count, out_support_info->formats));

    u32 previous_present_mode_count = out_support_info->present_mode_count;
    ORB_VK_EXPECT(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, surface, &out_support_info->present_mode_count, nullptr));

    if (out_support_info->present_mode_count == 0) {
        return false;
    }

    if (!out_support_info->present_modes) {
        out_support_info->present_modes = (VkPresentModeKHR *)orb_allocate(
            out_support_info->present_mode_count * sizeof(VkPresentModeKHR), MEMORY_TAG_RENDERER);
    } else {
        ORB_DEBUG_ASSERT(out_support_info->present_mode_count == previous_present_mode_count,
                         "amount of supported present modes must not change");
    }

    ORB_VK_EXPECT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, surface, &out_support_info->format_count, out_support_info->formats));

    return true;
}

b8 orb_vulkan_device_detect_depth_format(orb_vulkan_device *device) {
    const VkFormat preferred_formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (u32 i = 0; i < ORB_ARRAY_LENGTH(preferred_formats); ++i) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, preferred_formats[i],
                                            &properties);

        if (((properties.linearTilingFeatures & flags) |
             (properties.optimalTilingFeatures & flags)) == flags) {
            device->depth_format = preferred_formats[i];
            return true;
        }
    }

    device->depth_format = VK_FORMAT_UNDEFINED;

    return false;
}

u8 rank_physical_device(VkPhysicalDevice device, VkSurfaceKHR surface,
                        const VkPhysicalDeviceProperties *properties,
                        const VkPhysicalDeviceFeatures *features,
                        orb_vulkan_physical_device_queue_family_info *out_queue_info,
                        orb_vulkan_swapchain_support_info *out_swapchain_support_info) {

    u8 suitability = 1;

    out_queue_info->graphics_family_index = (u32)-1;
    out_queue_info->compute_family_index = (u32)-1;
    out_queue_info->transfer_family_index = (u32)-1;
    out_queue_info->present_family_index = (u32)-1;

    switch (properties->deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        suitability += 10;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        suitability += 5;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        break;
    default:
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        ORB_ERROR("Unknown physical device type: %s",
                  string_VkPhysicalDeviceType(properties->deviceType));
        return 0;
        break;
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    b8 has_pure_transfer_queue = false;
    for (u32 i = 0; i < queue_family_count; ++i) {
        VkQueueFamilyProperties *family = &queue_families[i];
        b8 is_pure_transfer_queue = true;

        if (family->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_info->graphics_family_index = i;
            out_queue_info->graphics_family_queue_count = family->queueCount;
            is_pure_transfer_queue = false;
        }

        if (family->queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_info->compute_family_index = i;
            out_queue_info->compute_family_queue_count = family->queueCount;
            is_pure_transfer_queue = false;
        }

        if (family->queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if (is_pure_transfer_queue || !has_pure_transfer_queue) {
                out_queue_info->transfer_family_index = i;
                out_queue_info->transfer_family_queue_count = family->queueCount;
                has_pure_transfer_queue = is_pure_transfer_queue;
            }
        }

        VkBool32 supports_present = VK_FALSE;
        ORB_VK_EXPECT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present) {
            out_queue_info->present_family_index = i;
        }
    }

    if (out_queue_info->graphics_family_index == (u32)-1 ||
        out_queue_info->compute_family_index == (u32)-1 ||
        out_queue_info->present_family_index == (u32)-1) {
        // this device does not support our needs
        return 0;
    }

    if (out_queue_info->transfer_family_index == (u32)-1) {
        // NOTE: according to the spec this should be fine, i made it a warning
        // because im not so sure if it really is. we will find out once a device
        // behaves like this.
        ORB_WARN("Device has no explicit transfer queue. Using graphics queue as "
                 "transfer queue.");
        out_queue_info->transfer_family_index = out_queue_info->graphics_family_index;
    }

    u32 device_extension_count = 0;
    ORB_VK_EXPECT(
        vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, nullptr));
    if (device_extension_count == 0) {
        return 0;
    }

    VkExtensionProperties device_extensions[device_extension_count];
    ORB_VK_EXPECT(vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count,
                                                       device_extensions));

    for (usize i = 0; i < ORB_ARRAY_LENGTH(device_required_extensions); ++i) {
        const char *required_extension = device_required_extensions[i];

        b8 found = false;

        for (usize j = 0; j < device_extension_count; ++j) {
            char *device_extension = device_extensions[j].extensionName;
            if (orb_string_equal(required_extension, device_extension)) {
                found = true;
                break;
            }
        }

        if (!found) {
            ORB_INFO("Device [%s] does not support required extension %s", properties->deviceName,
                     required_extension);
            return 0;
        }
    }

    if (!features->samplerAnisotropy) {
        ORB_INFO("Device [%s] does not support samplerAnisotropy", properties->deviceName);
        return 0;
    }

    if (!orb_vulkan_device_query_swapchain_support(device, surface, out_swapchain_support_info)) {
        // clean up allocations if swapchain support did not suffice
        if (out_swapchain_support_info->formats) {
            orb_free(out_swapchain_support_info->formats,
                     out_swapchain_support_info->format_count * sizeof(VkSurfaceFormatKHR),
                     MEMORY_TAG_RENDERER);
        }

        if (out_swapchain_support_info->present_modes) {
            orb_free(out_swapchain_support_info->present_modes,
                     out_swapchain_support_info->present_mode_count * sizeof(VkPresentModeKHR),
                     MEMORY_TAG_RENDERER);
        }

        return 0;
    };

    return suitability;
}
