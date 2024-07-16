#include "vulkan_backend.h"

#include "../../core/asserts.h"

#include "platform/vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_renderpass.h"
#include "vulkan_swapchain.h"
#include "vulkan_types.h"

static orb_vulkan_context context = {
    0,
    .allocator = nullptr,
};

#ifdef ORB_PLATFORM_MAC
const u32 instance_flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
const u32 instance_flags = 0;
#endif

const char *required_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifndef ORB_RELEASE
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#ifdef ORB_PLATFORM_MAC
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    "VK_EXT_metal_surface",
#elifdef ORB_PLATFORM_LINUX
    "VK_KHR_xcb_surface",
#endif

};

#ifdef ORB_RELEASE
const char **validation_layers = nullptr;
u32 validation_layers_count = 0;
#else
const char *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
u32 validation_layers_count = ORB_ARRAY_LENGTH(validation_layers);
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL
orb_vk_debug_callback(VkDebugUtilsMessageSeverityFlagsEXT message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT message_types,
                      const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                      void *user_data) {
  (void)user_data;
  (void)message_types;
  (void)callback_data;

  switch (message_severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    ORB_ERROR("%s", callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    ORB_WARN("%s", callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    ORB_INFO("%s", callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    ORB_TRACE("%s", callback_data->pMessage);
    break;
  }

  return VK_FALSE;
}

b8 vulkan_backend_initialize(orb_renderer_backend *backend,
                             const char *application_name,
                             struct orb_platform_state *platform_state) {
  (void)backend;
  (void)platform_state;
  VkApplicationInfo app_info = {
      VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .apiVersion = VK_API_VERSION_1_2,
      .pApplicationName = application_name,
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "orb",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
  };

  VkInstanceCreateInfo create_info = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .flags = instance_flags,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = ORB_ARRAY_LENGTH(required_extensions),
      .ppEnabledExtensionNames = required_extensions,
      .enabledLayerCount = validation_layers_count,
      .ppEnabledLayerNames = validation_layers,
  };

  ORB_VK_EXPECT(
      vkCreateInstance(&create_info, context.allocator, &context.instance));

#ifndef ORB_RELEASE
  ORB_DEBUG("Creating Vulkan debugger");
  u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
  // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

  u32 message_types =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = log_severity,
      .messageType = message_types,
      .pfnUserCallback = orb_vk_debug_callback,
  };

  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          context.instance, "vkCreateDebugUtilsMessengerEXT");

  ORB_DEBUG_ASSERT(vkCreateDebugUtilsMessenger,
                   "Vulkan debug extension could not be loaded");

  ORB_VK_EXPECT(
      vkCreateDebugUtilsMessenger(context.instance, &debug_create_info,
                                  context.allocator, &context.debug_messenger));
#endif

  ORB_DEBUG("Creating Vulkan surface");
  if (!orb_vulkan_platform_surface_init(&context)) {
    ORB_ERROR("Failed to create vulkan surface");
    return FALSE;
  };

  ORB_DEBUG("Creating Vulkan device");
  if (!orb_vulkan_device_init(&context)) {
    ORB_ERROR("Failed to create vulkan device");
    return FALSE;
  }

  ORB_DEBUG("Creating Vulkan swapchain");
  if (!orb_vulkan_swapchain_init(&context, context.framebuffer_width,
                                 context.framebuffer_height,
                                 &context.swapchain)) {
    ORB_ERROR("Failed to create vulkan swapchain");
    return FALSE;
  }

  if (!orb_vulkan_renderpass_create(
          &context, &context.main_renderpass, 0, 0, context.framebuffer_width,
          context.framebuffer_height, 0.0f, 0.1f, 0.3f, 1.0f, 1.0f, 0)) {
    ORB_ERROR("Failed to create renderpass");
    return FALSE;
  }

  ORB_INFO("Vulkan renderer initialized successfully.");

  return TRUE;
}

void vulkan_backend_shutdown(orb_renderer_backend *backend) {
  (void)backend;

  orb_vulkan_renderpass_destroy(&context, &context.main_renderpass);

  orb_vulkan_swapchain_shutdown(&context, &context.swapchain);

  if (context.surface) {
    vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
  }

  orb_vulkan_device_shutdown(&context);
#ifndef ORB_RELEASE
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          context.instance, "vkDestroyDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT(context.instance, context.debug_messenger,
                                  context.allocator);
#endif

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

u32 orb_vulkan_find_memory_index(u32 type_filter, u32 property_flags) {
  VkPhysicalDeviceMemoryProperties properties = context.device.memory;
  for (u32 i = 0; i < properties.memoryTypeCount; ++i) {
    if (type_filter & (1 << i) && (properties.memoryTypes[i].propertyFlags &
                                   property_flags) == property_flags) {
      return i;
    }
  }

  return UINT32_MAX;
}
