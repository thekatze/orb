#pragma once

#include "vulkan_types.h"

[[nodiscard]]
b8 orb_vulkan_swapchain_init(orb_vulkan_context *context, u32 width, u32 height,
                             orb_vulkan_swapchain *out_swapchain);

[[nodiscard]]
b8 orb_vulkan_swapchain_recreate(orb_vulkan_context *context, u32 width, u32 height,
                                 orb_vulkan_swapchain *swapchain);

void orb_vulkan_swapchain_shutdown(orb_vulkan_context *context, orb_vulkan_swapchain *swapchain);

[[nodiscard]]
b8 orb_vulkan_swapchain_acquire_next_image_index(orb_vulkan_context *context,
                                                 orb_vulkan_swapchain *swapchain, u64 timeout_ns,
                                                 VkSemaphore image_available_semaphore,
                                                 VkFence fence, u32 *out_image_index);

[[nodiscard]]
b8 orb_vulkan_swapchain_present(orb_vulkan_context *context, orb_vulkan_swapchain *swapchain,
                                VkQueue graphics_queue, VkQueue present_queue,
                                VkSemaphore render_complete_semaphore, u32 present_image_index);
