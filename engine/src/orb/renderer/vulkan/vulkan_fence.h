#pragma once

#include "vulkan_types.h"

[[nodiscard]]
b8 orb_vulkan_fence_create(orb_vulkan_context *context, b8 create_signalled,
                           orb_vulkan_fence *out_fence);

void orb_vulkan_fence_destroy(orb_vulkan_context *context, orb_vulkan_fence *fence);

[[nodiscard]]
b8 orb_vulkan_fence_wait(orb_vulkan_context *context, orb_vulkan_fence *fence, u64 timeout_ns);

[[nodiscard]]
b8 orb_vulkan_fence_reset(orb_vulkan_context *context, orb_vulkan_fence *fence);
