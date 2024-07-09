#pragma once

#include "../../core/types.h"

#include <vulkan/vulkan.h>

#include <vulkan/vk_enum_string_helper.h>
#define ORB_VK_EXPECT(vk_api_call)                                             \
  do {                                                                         \
    VkResult result = vk_api_call;                                             \
    if (result != VK_SUCCESS) {                                                \
      ORB_FATAL(#vk_api_call " failed with result: %s",                        \
                string_VkResult(result));                                      \
      return FALSE;                                                            \
    }                                                                          \
  } while (0)

typedef struct orb_vulkan_context {
  VkInstance instance;
  VkAllocationCallbacks *allocator;

#ifndef ORB_RELEASE
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
} orb_vulkan_context;
