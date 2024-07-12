#pragma once

#include "../../core/logger.h"
#include "../../core/types.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define ORB_VK_EXPECT(vk_api_call)                                             \
  do {                                                                         \
    VkResult result = vk_api_call;                                             \
    if (unlikely(result != VK_SUCCESS)) {                                      \
      ORB_FATAL(#vk_api_call " failed with result: %s",                        \
                string_VkResult(result));                                      \
      return FALSE;                                                            \
    }                                                                          \
  } while (0)

u32 orb_vulkan_find_memory_index(u32 type_filter, u32 property_flags);

typedef struct orb_vulkan_swapchain_support_info {
  VkSurfaceCapabilitiesKHR capabilities;
  u32 format_count;
  VkSurfaceFormatKHR *formats;
  u32 present_mode_count;
  VkPresentModeKHR *present_modes;
} orb_vulkan_swapchain_support_info;

typedef struct orb_vulkan_physical_device_queue_family_info {
  u32 graphics_family_index;
  u32 graphics_family_queue_count;
  u32 compute_family_index;
  u32 compute_family_queue_count;
  u32 transfer_family_index;
  u32 transfer_family_queue_count;

  u32 present_family_index;
} orb_vulkan_physical_device_queue_family_info;

typedef struct orb_vulkan_device {
  VkPhysicalDevice physical_device;
  VkDevice logical_device;

  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceMemoryProperties memory;

  VkFormat depth_format;

  orb_vulkan_swapchain_support_info swapchain;

  orb_vulkan_physical_device_queue_family_info queue_info;
  VkQueue graphics_queue;
  VkQueue compute_queue;
  VkQueue transfer_queue;
  VkQueue present_queue;

} orb_vulkan_device;

typedef struct orb_vulkan_image {
  VkImage handle;
  VkDeviceMemory memory;
  VkImageView view;
  u32 width;
  u32 height;
} orb_vulkan_image;

typedef struct orb_vulkan_swapchain {
  VkSwapchainKHR handle;

  VkSurfaceFormatKHR image_format;
  u32 image_count;
  VkImage *images;
  VkImageView *views;

  orb_vulkan_image depth_attachment;

  u8 max_frames_in_flight;
} orb_vulkan_swapchain;

typedef struct orb_vulkan_context {
  VkInstance instance;
  VkAllocationCallbacks *allocator;

  orb_vulkan_device device;
  VkSurfaceKHR surface;
  orb_vulkan_swapchain swapchain;
  u32 image_index;
  u32 current_frame;

  b8 recreating_swapchain;

  u32 framebuffer_width;
  u32 framebuffer_height;

#ifndef ORB_RELEASE
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
} orb_vulkan_context;
