#include "vulkan_image.h"

b8 orb_vulkan_image_create(orb_vulkan_context *context, VkImageType image_type, u32 width,
                           u32 height, VkFormat format, VkImageTiling tiling,
                           VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_flags,
                           b8 create_view, VkImageAspectFlags view_aspect_flags,
                           orb_vulkan_image *out_image) {
    out_image->width = width;
    out_image->height = height;

    VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = image_type,
        .extent = {.width = width, .height = height, .depth = 1},
        .mipLevels = 4,
        .arrayLayers = 1,
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage_flags,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ORB_VK_EXPECT(vkCreateImage(context->device.logical_device, &image_create_info,
                                context->allocator, &out_image->handle));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, out_image->handle,
                                 &memory_requirements);

    u32 memory_type =
        orb_vulkan_find_memory_index(memory_requirements.memoryTypeBits, memory_flags);

    if (memory_type == UINT32_MAX) {
        ORB_ERROR("Required memory type not available.");
        return FALSE;
    }

    VkMemoryAllocateInfo memory_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type,
    };

    ORB_VK_EXPECT(vkAllocateMemory(context->device.logical_device, &memory_allocate_info,
                                   context->allocator, &out_image->memory));
    ORB_VK_EXPECT(
        vkBindImageMemory(context->device.logical_device, out_image->handle, out_image->memory, 0));

    if (create_view) {
        out_image->view = 0;
        return orb_vulkan_image_view_create(context, format, out_image, view_aspect_flags);
    }

    return TRUE;
}

b8 orb_vulkan_image_view_create(orb_vulkan_context *context, VkFormat format,
                                orb_vulkan_image *image, VkImageAspectFlags view_aspect_flags) {
    VkImageViewCreateInfo view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image->handle,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange =
            {
                .aspectMask = view_aspect_flags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    ORB_VK_EXPECT(vkCreateImageView(context->device.logical_device, &view_create_info,
                                    context->allocator, &image->view));

    return TRUE;
}

void orb_vulkan_image_destroy(orb_vulkan_context *context, orb_vulkan_image *image) {
    if (image->view) {
        vkDestroyImageView(context->device.logical_device, image->view, context->allocator);
        image->view = 0;
    }

    if (image->memory) {
        vkFreeMemory(context->device.logical_device, image->memory, context->allocator);
        image->memory = 0;
    }

    if (image->handle) {
        vkDestroyImage(context->device.logical_device, image->handle, context->allocator);
        image->handle = 0;
    }
}
