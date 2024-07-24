#pragma once

#include "vulkan_types.h"

b8 orb_vulkan_image_create(orb_vulkan_context *context, VkImageType image_type, u32 width,
                           u32 height, VkFormat format, VkImageTiling tiling,
                           VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_flags,
                           b8 create_view, VkImageAspectFlags view_aspect_flags,
                           orb_vulkan_image *out_image);

b8 orb_vulkan_image_view_create(orb_vulkan_context *context, VkFormat format,
                                orb_vulkan_image *image, VkImageAspectFlags view_aspect_flags);

void orb_vulkan_image_destroy(orb_vulkan_context *context, orb_vulkan_image *image);
