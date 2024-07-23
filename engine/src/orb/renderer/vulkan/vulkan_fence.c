#include "vulkan_fence.h"

b8 orb_vulkan_fence_create(orb_vulkan_context *context, b8 create_signalled,
                           orb_vulkan_fence *out_fence) {
  out_fence->is_signalled = create_signalled;

  VkFenceCreateInfo fence_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = create_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0,
  };

  ORB_VK_EXPECT(vkCreateFence(context->device.logical_device, &fence_info,
                              context->allocator, &out_fence->handle));
  return TRUE;
}

void orb_vulkan_fence_destroy(orb_vulkan_context *context,
                              orb_vulkan_fence *fence) {
  if (fence->handle) {
    vkDestroyFence(context->device.logical_device, fence->handle,
                   context->allocator);
    fence->handle = 0;
  }
}

b8 orb_vulkan_fence_wait(orb_vulkan_context *context, orb_vulkan_fence *fence,
                         u64 timeout_ns) {
  ORB_VK_EXPECT(vkWaitForFences(context->device.logical_device, 1,
                                &fence->handle, TRUE, timeout_ns));
  fence->is_signalled = TRUE;
  return TRUE;
}

b8 orb_vulkan_fence_reset(orb_vulkan_context *context,
                          orb_vulkan_fence *fence) {
  if (fence->is_signalled) {
    ORB_VK_EXPECT(
        vkResetFences(context->device.logical_device, 1, &fence->handle));
    fence->is_signalled = FALSE;
  }

  return TRUE;
}
