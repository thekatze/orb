#pragma once

#include "vulkan_types.h"

b8 orb_vulkan_framebuffer_create(orb_vulkan_context *context, orb_vulkan_renderpass *renderpass,
                                 u32 width, u32 height, u32 attachment_count,
                                 VkImageView *attachments, orb_vulkan_framebuffer *out_framebuffer);

void orb_vulkan_framebuffer_destroy(orb_vulkan_context *context,
                                    orb_vulkan_framebuffer *framebuffer);
