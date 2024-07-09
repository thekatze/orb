#include "vulkan_backend.h"

#include "../../core/asserts.h"
#include "../../core/logger.h"

#include "vulkan_types.h"

static orb_vulkan_context context = {
    0,
    .allocator = 0,
};

const char *required_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifndef ORB_RELEASE
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#ifdef ORB_PLATFORM_MAC
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    (const char *)"VK_MVK_macos_surface",
#endif
};

#ifdef ORB_PLATFORM_MAC
const u32 instance_flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
const u32 instance_flags = 0;
#endif

const char *validation_layers[] =
#ifdef ORB_RELEASE
    0;
#else
    {"VK_LAYER_KHRONOS_validation"};
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
      .enabledLayerCount = ORB_ARRAY_LENGTH(validation_layers),
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

  ORB_INFO("Vulkan renderer initialized successfully.");

  return TRUE;
}

void vulkan_backend_shutdown(orb_renderer_backend *backend) {
  (void)backend;

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
