#include "vulkan_backend.h"

#include "../../core/logger.h"
#include "vulkan_types.h"

#include <vulkan/vk_enum_string_helper.h>

static orb_vulkan_context context = {
    0,
    .allocator = 0,
};

b8 vulkan_backend_initialize(orb_renderer_backend *backend,
                             const char *application_name,
                             struct orb_platform_state *platform_state) {
  (void)backend;
  (void)platform_state;
  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .apiVersion = VK_API_VERSION_1_2,
      .pApplicationName = application_name,
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "orb",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
  };

  VkInstanceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = 0,
      .ppEnabledExtensionNames = 0,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = 0,
  };

  VkResult result =
      vkCreateInstance(&create_info, context.allocator, &context.instance);
  if (result != VK_SUCCESS) {
    ORB_FATAL("vkCreateInstance failed with result: %s",
              string_VkResult(result));
    return FALSE;
  }

  ORB_INFO("Vulkan renderer initialized successfully.");

  return TRUE;
}

void vulkan_backend_shutdown(orb_renderer_backend *backend) {
  (void)backend;
  vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_backend_resize(orb_renderer_backend *backend, u16 width,
                           u16 height) {
  (void)backend;
  (void)width;
  (void)height;
}

b8 vulkan_backend_begin_frame(orb_renderer_backend *backend, f32 delta_time) {
  (void)backend;
  (void)delta_time;
  return TRUE;
}

b8 vulkan_backend_end_frame(orb_renderer_backend *backend, f32 delta_time) {
  (void)backend;
  (void)delta_time;
  return TRUE;
}
