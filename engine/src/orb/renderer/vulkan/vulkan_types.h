#pragma once

#include "../../core/logger.h"
#include "../../core/types.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define ORB_VK_EXPECT(vk_api_call)                                             \
  do {                                                                         \
    VkResult result = vk_api_call;                                             \
    if (result != VK_SUCCESS) {                                                \
      ORB_FATAL(#vk_api_call " failed with result: %s",                        \
                string_VkResult(result));                                      \
      return FALSE;                                                            \
    }                                                                          \
  } while (0)

typedef struct orb_vulkan_swapchain_support_info {
  VkSurfaceCapabilitiesKHR capabilities;
  u32 format_count;
  VkSurfaceFormatKHR *formats;
  u32 present_mode_count;
  VkPresentModeKHR *present_modes;
} orb_vulkan_swapchain_support_info;

typedef struct orb_vulkan_physical_device_queue_family_info {
  u32 graphics_family_index;
  u32 present_family_index;
  u32 compute_family_index;
  u32 transfer_family_index;
} orb_vulkan_physical_device_queue_family_info;

typedef struct orb_vulkan_device {
  VkPhysicalDevice physical_device;
  VkDevice logical_device;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceMemoryProperties memory;
  orb_vulkan_physical_device_queue_family_info queues;
  orb_vulkan_swapchain_support_info swapchain;
} orb_vulkan_device;

typedef struct orb_vulkan_context {
  VkInstance instance;
  VkAllocationCallbacks *allocator;
  orb_vulkan_device device;
  VkSurfaceKHR surface;

#ifndef ORB_RELEASE
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
} orb_vulkan_context;
