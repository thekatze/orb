#include "vulkan_swapchain.h"

#include "../../core/asserts.h"
#include "../../core/logger.h"
#include "../../core/orb_memory.h"
#include "vulkan_device.h"
#include "vulkan_image.h"

b8 create_swapchain(orb_vulkan_context *context, u32 width, u32 height,
                    orb_vulkan_swapchain *out_swapchain);
void cleanup_swapchain(orb_vulkan_context *context, orb_vulkan_swapchain *swapchain);

b8 orb_vulkan_swapchain_init(orb_vulkan_context *context, u32 width, u32 height,
                             orb_vulkan_swapchain *out_swapchain) {
    return create_swapchain(context, width, height, out_swapchain);
}

b8 orb_vulkan_swapchain_recreate(orb_vulkan_context *context, u32 width, u32 height,
                                 orb_vulkan_swapchain *swapchain) {
    cleanup_swapchain(context, swapchain);
    return create_swapchain(context, width, height, swapchain);
}

void orb_vulkan_swapchain_shutdown(orb_vulkan_context *context, orb_vulkan_swapchain *swapchain) {
    cleanup_swapchain(context, swapchain);
}

b8 orb_vulkan_swapchain_acquire_next_image_index(orb_vulkan_context *context,
                                                 orb_vulkan_swapchain *swapchain, u64 timeout_ns,
                                                 VkSemaphore image_available_semaphore,
                                                 VkFence fence, u32 *out_image_index) {
    VkResult result =
        vkAcquireNextImageKHR(context->device.logical_device, swapchain->handle, timeout_ns,
                              image_available_semaphore, fence, out_image_index);

    switch (result) {
    case VK_SUCCESS:
        break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR:
        orb_vulkan_swapchain_recreate(context, context->framebuffer_width,
                                      context->framebuffer_height, swapchain);
        return FALSE;
    default:
        ORB_FATAL("vkAcquireNextImageKHR failed with result: %s", string_VkResult(result));
        return FALSE;
    }

    return TRUE;
}

b8 orb_vulkan_swapchain_present(orb_vulkan_context *context, orb_vulkan_swapchain *swapchain,
                                VkQueue graphics_queue, VkQueue present_queue,
                                VkSemaphore render_complete_semaphore, u32 present_image_index) {
    (void)graphics_queue;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_complete_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain->handle,
        .pImageIndices = &present_image_index,
    };

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);

    switch (result) {
    case VK_SUCCESS:
        break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR:
        orb_vulkan_swapchain_recreate(context, context->framebuffer_width,
                                      context->framebuffer_height, swapchain);
        break;
    default:
        ORB_FATAL("vkAcquireNextImageKHR failed with result: %s", string_VkResult(result));
        return FALSE;
    }

    context->current_frame = (context->current_frame + 1) % swapchain->max_frames_in_flight;

    return TRUE;
}

b8 create_swapchain(orb_vulkan_context *context, u32 width, u32 height,
                    orb_vulkan_swapchain *out_swapchain) {
    out_swapchain->max_frames_in_flight = 2;

    b8 found = FALSE;
    for (u32 i = 0; i < context->device.swapchain.format_count; ++i) {
        VkSurfaceFormatKHR *format = &context->device.swapchain.formats[i];
        if (format->format == VK_FORMAT_B8G8R8A8_UNORM &&
            format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            out_swapchain->image_format = *format;
            found = TRUE;
            break;
        }
    }

    if (!found) {
        out_swapchain->image_format = context->device.swapchain.formats[0];
        ORB_ERROR("Graphics device does not support preferred image format. "
                  "Defaulting to %s %s",
                  string_VkFormat(out_swapchain->image_format.format),
                  string_VkColorSpaceKHR(out_swapchain->image_format.colorSpace));
    }

    // guaranteed to exist, so we default to FIFO
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < context->device.swapchain.present_mode_count; ++i) {
        VkPresentModeKHR mode = context->device.swapchain.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = mode;
            break;
        }
    }

    // NOTE: i would do this query at the beginning of creation, but i will keep
    // it here for now

    orb_vulkan_device_query_swapchain_support(context->device.physical_device, context->surface,
                                              &context->device.swapchain);

    VkExtent2D swapchain_extent = {
        .width = width,
        .height = height,
    };

    auto capabilities = context->device.swapchain.capabilities;

    if (capabilities.currentExtent.width != UINT32_MAX) {
        swapchain_extent = capabilities.currentExtent;
    }

    VkExtent2D min = capabilities.minImageExtent;
    VkExtent2D max = capabilities.maxImageExtent;
    swapchain_extent.width = ORB_CLAMP(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = ORB_CLAMP(swapchain_extent.height, min.height, max.height);

    u32 desired_image_count = capabilities.minImageCount + 1;
    u32 image_count = capabilities.maxImageCount == 0 // 0 means unlimited, according to spec
                          ? desired_image_count
                          : ORB_MIN(desired_image_count, capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = context->surface,
        .minImageCount = image_count,
        .imageFormat = out_swapchain->image_format.format,
        .imageColorSpace = out_swapchain->image_format.colorSpace,
        .imageExtent = swapchain_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = context->device.swapchain.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = 0, // TODO: when recreating pass old swapchain here

        // initialize to single queue swapchain
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    u32 queueFamilyIndices[] = {
        context->device.queue_info.graphics_family_index,
        context->device.queue_info.present_family_index,
    };

    // if we can, we use separate queues
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = ORB_ARRAY_LENGTH(queueFamilyIndices);
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
    }

    ORB_VK_EXPECT(vkCreateSwapchainKHR(context->device.logical_device, &swapchain_create_info,
                                       context->allocator, &out_swapchain->handle));

    context->current_frame = 0;
    out_swapchain->image_count = 0;
    ORB_VK_EXPECT(vkGetSwapchainImagesKHR(context->device.logical_device, out_swapchain->handle,
                                          &out_swapchain->image_count, nullptr));

    if (out_swapchain->image_count == 0) {
        ORB_ERROR("getting swapchain images failed");
        return FALSE;
    }

    if (!out_swapchain->images) {
        out_swapchain->images = (VkImage *)orb_allocate(
            sizeof(VkImage) * out_swapchain->image_count, MEMORY_TAG_RENDERER);
    }

    if (!out_swapchain->views) {
        out_swapchain->views = (VkImageView *)orb_allocate(
            sizeof(VkImageView) * out_swapchain->image_count, MEMORY_TAG_RENDERER);
    }

    ORB_VK_EXPECT(vkGetSwapchainImagesKHR(context->device.logical_device, out_swapchain->handle,
                                          &out_swapchain->image_count, out_swapchain->images));

    for (u32 i = 0; i < out_swapchain->image_count; ++i) {
        VkImageViewCreateInfo view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = out_swapchain->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = out_swapchain->image_format.format,
            .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };

        ORB_VK_EXPECT(vkCreateImageView(context->device.logical_device, &view_create_info,
                                        context->allocator, &out_swapchain->views[i]));
    }

    // depth buffer
    if (!orb_vulkan_device_detect_depth_format(&context->device)) {
        ORB_ERROR("Device does not support a suitable depth buffer format");
        return FALSE;
    }

    if (!orb_vulkan_image_create(
            context, VK_IMAGE_TYPE_2D, swapchain_extent.width, swapchain_extent.height,
            context->device.depth_format, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, TRUE,
            VK_IMAGE_ASPECT_DEPTH_BIT, &out_swapchain->depth_attachment)) {
        ORB_ERROR("Depth buffer image could not be created.");
        return FALSE;
    };

    return TRUE;
}

void cleanup_swapchain(orb_vulkan_context *context, orb_vulkan_swapchain *swapchain) {
    vkDeviceWaitIdle(context->device.logical_device);

    // NOTE: we might need to set swapchain_images and views to 0
    orb_vulkan_image_destroy(context, &swapchain->depth_attachment);

    if (swapchain->images) {
        swapchain->images = 0;
        orb_free(swapchain->images, sizeof(VkImage) * swapchain->image_count, MEMORY_TAG_RENDERER);
    }

    if (swapchain->views) {
        // only destroy the views. the images are owned by the swapchain
        for (u32 i = 0; i < swapchain->image_count; ++i) {
            vkDestroyImageView(context->device.logical_device, swapchain->views[i],
                               context->allocator);
        }

        orb_free(swapchain->views, sizeof(VkImageView) * swapchain->image_count,
                 MEMORY_TAG_RENDERER);

        swapchain->views = 0;
    }

    vkDestroySwapchainKHR(context->device.logical_device, swapchain->handle, context->allocator);
}
