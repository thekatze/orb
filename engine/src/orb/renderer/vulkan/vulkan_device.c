#include "vulkan_device.h"
#include "vulkan_types.h"

b8 select_physical_device(orb_vulkan_context *context);

b8 orb_vulkan_device_init(orb_vulkan_context *context) {
  if (!select_physical_device(context)) {
    return FALSE;
  }

  return TRUE;
}

void orb_vulkan_device_shutdown(orb_vulkan_context *context) { (void)context; }

b8 select_physical_device(orb_vulkan_context *context) {
  u32 physical_device_count = 0;
  ORB_VK_EXPECT(vkEnumeratePhysicalDevices(context->instance,
                                           &physical_device_count, nullptr));

  if (physical_device_count == 0) {
    ORB_FATAL("No Vulkan capable devices found.");
    return FALSE;
  }

  VkPhysicalDevice physical_devices[physical_device_count];
  ORB_VK_EXPECT(vkEnumeratePhysicalDevices(
      context->instance, &physical_device_count, physical_devices));

  return TRUE;
}
