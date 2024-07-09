#pragma once

#include "../../core/types.h"

#include <vulkan/vulkan.h>

typedef struct orb_vulkan_context {
  VkInstance instance;
  VkAllocationCallbacks *allocator;
} orb_vulkan_context;
