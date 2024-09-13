#pragma once

#include "vulkan_types.h"

[[nodiscard]]
b8 orb_vulkan_device_init(orb_vulkan_context *context);
void orb_vulkan_device_shutdown(orb_vulkan_context *context);
[[nodiscard]]
b8 orb_vulkan_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                                             orb_vulkan_swapchain_support_info *out_support_info);
[[nodiscard]]
b8 orb_vulkan_device_detect_depth_format(orb_vulkan_device *device);
