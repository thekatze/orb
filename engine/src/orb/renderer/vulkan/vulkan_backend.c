#include "vulkan_backend.h"

#include "../../core/application.h"
#include "../../core/asserts.h"
#include "../../core/orb_memory.h"

#include "platform/vulkan_platform.h"
#include "shaders/vulkan_object_shader.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_fence.h"
#include "vulkan_framebuffer.h"
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
#elifdef ORB_PLATFORM_WINDOWS
    "VK_KHR_win32_surface",
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
orb_vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT message_types,
                      const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
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
    default:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        ORB_INFO("%s", callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        ORB_TRACE("%s", callback_data->pMessage);
        break;
    }

    return VK_FALSE;
}

b8 create_command_buffers(orb_renderer_backend *backend);
b8 regenerate_framebuffers(orb_renderer_backend *backend, orb_vulkan_swapchain *swapchain,
                           orb_vulkan_renderpass *renderpass);
b8 recreate_swapchain(orb_renderer_backend *backend);

b8 vulkan_backend_initialize(orb_renderer_backend *backend,
                             orb_application_config *application_config) {
    (void)backend;

    context.framebuffer_width = application_config->width;
    context.framebuffer_height = application_config->height;

    VkApplicationInfo app_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_2,
        .pApplicationName = application_config->name,
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

    ORB_VK_EXPECT(vkCreateInstance(&create_info, context.allocator, &context.instance));

#ifndef ORB_RELEASE
    ORB_DEBUG("Creating Vulkan debugger");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    u32 message_types = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
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
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance,
                                                                  "vkCreateDebugUtilsMessengerEXT");

    ORB_DEBUG_ASSERT(vkCreateDebugUtilsMessenger, "Vulkan debug extension could not be loaded");

    ORB_VK_EXPECT(vkCreateDebugUtilsMessenger(context.instance, &debug_create_info,
                                              context.allocator, &context.debug_messenger));
#endif

    ORB_DEBUG("Creating Vulkan surface");
    if (!orb_vulkan_platform_surface_init(&context)) {
        ORB_ERROR("Failed to create vulkan surface");
        return false;
    };

    ORB_DEBUG("Creating Vulkan device");
    if (!orb_vulkan_device_init(&context)) {
        ORB_ERROR("Failed to create vulkan device");
        return false;
    }

    ORB_DEBUG("Creating Vulkan swapchain");
    if (!orb_vulkan_swapchain_init(&context, context.framebuffer_width, context.framebuffer_height,
                                   &context.swapchain)) {
        ORB_ERROR("Failed to create vulkan swapchain");
        return false;
    }

    ORB_DEBUG("Creating Vulkan renderpass");
    if (!orb_vulkan_renderpass_create(&context, &context.main_renderpass, 0, 0,
                                      context.framebuffer_width, context.framebuffer_height, 0.0f,
                                      0.1f, 0.3f, 1.0f, 1.0f, 0)) {
        ORB_ERROR("Failed to create renderpass");
        return false;
    }

    context.swapchain.framebuffers = orb_allocate(
        sizeof(orb_vulkan_framebuffer) * context.swapchain.image_count, MEMORY_TAG_RENDERER);

    ORB_DEBUG("Creating Vulkan framebuffers");
    if (!regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass)) {
        ORB_ERROR("Failed to create framebuffers");
        return false;
    }

    ORB_DEBUG("Creating Vulkan command buffers");
    if (!create_command_buffers(backend)) {
        ORB_ERROR("Failed to create command buffers");
        return false;
    }

    ORB_DEBUG("Creating Vulkan synchronization primitives");
    u8 max_frames_in_flight = context.swapchain.max_frames_in_flight;

    context.image_available_semaphores =
        orb_allocate(sizeof(VkSemaphore) * max_frames_in_flight, MEMORY_TAG_RENDERER);
    context.queue_complete_semaphores =
        orb_allocate(sizeof(VkSemaphore) * max_frames_in_flight, MEMORY_TAG_RENDERER);

    context.in_flight_fences =
        orb_allocate(sizeof(orb_vulkan_fence) * max_frames_in_flight, MEMORY_TAG_RENDERER);

    for (u8 i = 0; i < max_frames_in_flight; ++i) {
        VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        ORB_VK_EXPECT(vkCreateSemaphore(context.device.logical_device, &semaphore_create_info,
                                        context.allocator, &context.image_available_semaphores[i]));

        ORB_VK_EXPECT(vkCreateSemaphore(context.device.logical_device, &semaphore_create_info,
                                        context.allocator, &context.queue_complete_semaphores[i]));

        // create the fence in a signalled state. the "first frame" already has been
        // rendered so the next frame (the actual first frame) may start
        if (!orb_vulkan_fence_create(&context, true, &context.in_flight_fences[i])) {
            ORB_ERROR("Failed to create fence");
            return false;
        };
    }

    context.images_in_flight = orb_allocate(
        sizeof(orb_vulkan_fence *) * context.swapchain.image_count, MEMORY_TAG_RENDERER);
    orb_memory_zero(context.images_in_flight,
                    sizeof(*context.images_in_flight) * context.swapchain.image_count);

    ORB_DEBUG("Creating default shader");
    if (!orb_vulkan_object_shader_create(&context, &context.object_shader)) {
        ORB_ERROR("Failed to create default shader");
        return false;
    }

    ORB_INFO("Vulkan renderer initialized successfully.");

    return true;
}

void vulkan_backend_shutdown(orb_renderer_backend *backend) {
    (void)backend;

    // wait for all operations to finish
    vkDeviceWaitIdle(context.device.logical_device);

    orb_vulkan_object_shader_destroy(&context, &context.object_shader);

    u8 max_frames_in_flight = context.swapchain.max_frames_in_flight;
    for (u8 i = 0; i < max_frames_in_flight; ++i) {
        if (context.image_available_semaphores[i]) {
            vkDestroySemaphore(context.device.logical_device, context.image_available_semaphores[i],
                               context.allocator);
            context.image_available_semaphores[i] = 0;
        }
        if (context.queue_complete_semaphores[i]) {
            vkDestroySemaphore(context.device.logical_device, context.queue_complete_semaphores[i],
                               context.allocator);
            context.queue_complete_semaphores[i] = 0;
        }

        orb_vulkan_fence_destroy(&context, &context.in_flight_fences[i]);
    }

    orb_free(context.image_available_semaphores, sizeof(VkSemaphore) * max_frames_in_flight,
             MEMORY_TAG_RENDERER);

    orb_free(context.queue_complete_semaphores, sizeof(VkSemaphore) * max_frames_in_flight,
             MEMORY_TAG_RENDERER);

    orb_free(context.in_flight_fences, sizeof(orb_vulkan_fence) * max_frames_in_flight,
             MEMORY_TAG_RENDERER);

    orb_free(context.images_in_flight, sizeof(orb_vulkan_fence *) * context.swapchain.image_count,
             MEMORY_TAG_RENDERER);

    if (context.swapchain.framebuffers != 0) {
        for (u32 i = 0; i < context.swapchain.image_count; ++i) {
            orb_vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
        }

        orb_free(context.swapchain.framebuffers,
                 sizeof(orb_vulkan_framebuffer) * context.swapchain.image_count,
                 MEMORY_TAG_RENDERER);
    }

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
    vkDestroyDebugUtilsMessengerEXT(context.instance, context.debug_messenger, context.allocator);
#endif

    vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_backend_resize(orb_renderer_backend *backend, u16 width, u16 height) {
    (void)backend;

    context.framebuffer_width = width;
    context.framebuffer_height = height;
    context.framebuffer_size_generation += 1;

    ORB_DEBUG("Vulkan renderer resized: %ix%i - Generation %u", width, height,
              context.framebuffer_size_generation);
}

b8 vulkan_backend_begin_frame(orb_renderer_backend *backend, f32 delta_time) {
    (void)backend;
    (void)delta_time;

    orb_vulkan_device *device = &context.device;

    if (context.recreating_swapchain) {
        ORB_INFO("Rendering frame skipped, swapchain recreated");
        ORB_VK_EXPECT(vkDeviceWaitIdle(device->logical_device));
        return false;
    }

    if (context.framebuffer_size_generation != context.framebuffer_size_last_generation) {
        ORB_INFO("Framebuffer out of date, recreating swapchain");
        ORB_VK_EXPECT(vkDeviceWaitIdle(device->logical_device));

        if (!recreate_swapchain(backend)) {
            return false;
        }

        return false;
    }

    if (!orb_vulkan_fence_wait(&context, &context.in_flight_fences[context.current_frame],
                               1 * 1'000'000'000)) // one second in nanoseconds
    {
        ORB_WARN("Timeout waiting for current frame fence");
        return false;
    }

    if (!orb_vulkan_swapchain_acquire_next_image_index(
            &context, &context.swapchain, UINT64_MAX,
            context.image_available_semaphores[context.current_frame], nullptr,
            &context.image_index)) {
        ORB_WARN("Could not acquire next image index");
        return false;
    }

    // Begin recording commands
    orb_vulkan_command_buffer *command_buffer =
        &context.graphics_command_buffers[context.image_index];

    orb_vulkan_command_buffer_reset(command_buffer);
    orb_vulkan_command_buffer_begin(command_buffer, 0);

    // TODO: test if we really need this, i don't want multiple backends and im
    // fine with this not having opengl compat
    // {
    //   VkViewport viewport = {
    //       .x = 0.0f,
    //       .y = (f32)context.framebuffer_height,
    //       .width = (f32)context.framebuffer_width,
    //       .height = (f32)-context.framebuffer_height,
    //       .minDepth = 0.0f,
    //       .maxDepth = 0.0f,
    //   };

    //   vkCmdSetViewport(command_buffer->handle, 0, 1, &viewport);

    //   VkRect2D scissor = {
    //       .offset =
    //           {
    //               .x = 0,
    //               .y = 0,
    //           },
    //       .extent =
    //           {
    //               .width = context.framebuffer_width,
    //               .height = context.framebuffer_height,
    //           },
    //   };

    //   vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);
    // }

    // TODO: we dont need to do this every frame, find a better spot for this
    // (resize?)
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    orb_vulkan_renderpass_begin(command_buffer, &context.main_renderpass,
                                context.swapchain.framebuffers[context.image_index].handle);

    return true;
}

b8 vulkan_backend_end_frame(orb_renderer_backend *backend, f32 delta_time) {
    (void)backend;
    (void)delta_time;

    orb_vulkan_command_buffer *command_buffer =
        &context.graphics_command_buffers[context.image_index];

    orb_vulkan_renderpass_end(command_buffer, &context.main_renderpass);
    orb_vulkan_command_buffer_end(command_buffer);

    // make sure the previous frame is not using this image
    if (context.images_in_flight[context.image_index] != VK_NULL_HANDLE) {
        orb_vulkan_fence_wait(&context, context.images_in_flight[context.image_index], UINT64_MAX);
    }

    // mark the image as in-use by this frame
    context.images_in_flight[context.image_index] =
        &context.in_flight_fences[context.current_frame];

    orb_vulkan_fence_reset(&context, &context.in_flight_fences[context.current_frame]);

    // has to be equal to waitSemaphoreCount
    VkPipelineStageFlags flags[1] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer->handle,

        // semaphores to be signalled when the queue is complete
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &context.queue_complete_semaphores[context.current_frame],

        // semaphores to wait on before the operation can begin
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &context.image_available_semaphores[context.current_frame],
        // each semaphore waits on the corresponding pipeline stage to complete
        .pWaitDstStageMask = flags,
    };

    ORB_VK_EXPECT(vkQueueSubmit(context.device.graphics_queue, 1, &submit_info,
                                context.in_flight_fences[context.current_frame].handle));

    orb_vulkan_command_buffer_update_submitted(command_buffer);

    if (!orb_vulkan_swapchain_present(&context, &context.swapchain, context.device.graphics_queue,
                                      context.device.present_queue,
                                      context.queue_complete_semaphores[context.current_frame],
                                      context.image_index)) {
        ORB_ERROR("Failed presenting image");
        return false;
    };

    return true;
}

u32 orb_vulkan_find_memory_index(u32 type_filter, u32 property_flags) {
    VkPhysicalDeviceMemoryProperties properties = context.device.memory;
    for (u32 i = 0; i < properties.memoryTypeCount; ++i) {
        if (type_filter & (1 << i) &&
            (properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
            return i;
        }
    }

    return UINT32_MAX;
}

b8 create_command_buffers(orb_renderer_backend *backend) {
    (void)backend;

    if (!context.graphics_command_buffers) {
        context.graphics_command_buffers = orb_allocate(
            sizeof(orb_vulkan_command_buffer) * context.swapchain.image_count, MEMORY_TAG_RENDERER);
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        orb_memory_zero(&context.graphics_command_buffers[i], sizeof(orb_vulkan_command_buffer));

        if (!orb_vulkan_command_buffer_allocate(&context, context.device.graphics_command_pool,
                                                true, &context.graphics_command_buffers[i])) {
            return false;
        };
    }

    return true;
}

b8 regenerate_framebuffers(orb_renderer_backend *backend, orb_vulkan_swapchain *swapchain,
                           orb_vulkan_renderpass *renderpass) {
    (void)backend;

    for (u32 i = 0; i < swapchain->image_count; ++i) {
        VkImageView attachments[] = {
            swapchain->views[i],
            swapchain->depth_attachment.view,
        };

        if (!orb_vulkan_framebuffer_create(
                &context, renderpass, context.framebuffer_width, context.framebuffer_height,
                ORB_ARRAY_LENGTH(attachments), attachments, &context.swapchain.framebuffers[i])) {
            return false;
        }
    }

    return true;
}

b8 recreate_swapchain(orb_renderer_backend *backend) {
    (void)backend;
    if (context.recreating_swapchain) {
        ORB_DEBUG("Already recreating swapchain");
        return false;
    }

    if (context.framebuffer_width == 0 || context.framebuffer_height == 0) {
        ORB_DEBUG("Swapchain can not be created with width or height 0");
        return false;
    }

    context.recreating_swapchain = true;

    ORB_VK_EXPECT(vkDeviceWaitIdle(context.device.logical_device));

    // clear images in flight
    orb_memory_zero(context.images_in_flight,
                    sizeof(*context.images_in_flight) * context.swapchain.image_count);

    orb_vulkan_device_query_swapchain_support(context.device.physical_device, context.surface,
                                              &context.device.swapchain);

    orb_vulkan_device_detect_depth_format(&context.device);

    orb_vulkan_swapchain_recreate(&context, context.framebuffer_width, context.framebuffer_height,
                                  &context.swapchain);

    // cleanup swapchain
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        orb_vulkan_command_buffer_free(&context, context.device.graphics_command_pool,
                                       &context.graphics_command_buffers[i]);
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        orb_vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    context.main_renderpass.x = 0;
    context.main_renderpass.y = 0;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    create_command_buffers(backend);

    context.framebuffer_size_last_generation = context.framebuffer_size_generation;
    context.recreating_swapchain = false;

    return true;
}
