#pragma once

#include "../../containers/dynamic_array.h"
#include "../../core/logger.h"
#include "../../core/types.h"

#ifndef ORB_PLATFORM_WINDOWS
#include <vulkan/vk_enum_string_helper.h>
#else
// TODO: fix the error that occurs when importing <vulkan/vk_enum_string_helper.h> on windows
#define string_VkResult(param) "Can not get string representation of VkResult"
#define string_VkFormat(param) "Can not get string representation of VkFormat"
#define string_VkColorSpaceKHR(param) "Can not get string representation of VkColorSpaceKHR"
#endif

#include <vulkan/vulkan.h>

#define ORB_VK_EXPECT(vk_api_call)                                                                 \
    do {                                                                                           \
        VkResult result = vk_api_call;                                                             \
        if (unlikely(result != VK_SUCCESS)) {                                                      \
            ORB_FATAL(#vk_api_call " failed with result: %s", string_VkResult(result));            \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define ORB_INVALID_INDEX 4294967295U

u32 orb_vulkan_find_memory_index(u32 type_filter, u32 property_flags);

typedef struct orb_vulkan_buffer {
    VkBuffer handle;
    usize total_size;
    VkBufferUsageFlagBits usage;
    b8 is_locked;
    VkDeviceMemory memory;
    u32 memory_index;
    u32 memory_property_flags;
} orb_vulkan_buffer;

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

    VkCommandPool graphics_command_pool;

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

typedef enum orb_vulkan_command_buffer_state {
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED,
} orb_vulkan_command_buffer_state;

typedef struct orb_vulkan_command_buffer {
    VkCommandBuffer handle;

    orb_vulkan_command_buffer_state state;
} orb_vulkan_command_buffer;

typedef enum orb_vulkan_renderpass_state {
    RENDERPASS_STATE_READY,
    RENDERPASS_STATE_RECORDING,
    RENDERPASS_STATE_IN_RENDER_PASS,
    RENDERPASS_STATE_RECORDING_ENDED,
    RENDERPASS_STATE_SUBMITTED,
    RENDERPASS_STATE_NOT_ALLOCATED,
} orb_vulkan_renderpass_state;

typedef struct orb_vulkan_renderpass {
    VkRenderPass handle;
    i32 x, y;       // TODO: vec2
    u32 w, h;       // TODO: vec2
    f32 r, g, b, a; // TODO: vec4

    f32 depth;
    u32 stencil;

    orb_vulkan_renderpass_state state;
} orb_vulkan_renderpass;

typedef struct orb_vulkan_framebuffer {
    VkFramebuffer handle;
    u32 attachment_count;
    VkImageView *attachments;
    orb_vulkan_renderpass *renderpass;
} orb_vulkan_framebuffer;

typedef struct orb_vulkan_swapchain {
    VkSwapchainKHR handle;

    VkSurfaceFormatKHR image_format;
    u32 image_count;
    VkImage *images;
    VkImageView *views;

    orb_vulkan_image depth_attachment;
    orb_vulkan_framebuffer *framebuffers;

    u8 max_frames_in_flight;
} orb_vulkan_swapchain;

typedef struct orb_vulkan_fence {
    VkFence handle;
    b8 is_signalled;
} orb_vulkan_fence;

typedef struct orb_vulkan_shader_stage {
    VkShaderModuleCreateInfo module_create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
} orb_vulkan_shader_stage;

typedef struct orb_vulkan_pipeline {
    VkPipeline handle;
    VkPipelineLayout layout;
} orb_vulkan_pipeline;

// {vertex, fragment}
#define ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT 2

typedef struct orb_vulkan_object_shader {
    orb_vulkan_shader_stage stages[ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT];

    orb_vulkan_pipeline pipeline;
} orb_vulkan_object_shader;

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
    u32 framebuffer_size_generation;
    u32 framebuffer_size_last_generation;

    orb_vulkan_renderpass main_renderpass;

    orb_vulkan_buffer object_vertex_buffer;
    orb_vulkan_buffer object_index_buffer;

    usize geometry_vertex_offset;
    usize geometry_index_offset;

    orb_vulkan_command_buffer *graphics_command_buffers;

    VkSemaphore *image_available_semaphores;
    VkSemaphore *queue_complete_semaphores;

    orb_vulkan_fence *in_flight_fences;

    orb_vulkan_fence **images_in_flight;

    orb_vulkan_object_shader object_shader;

#ifndef ORB_RELEASE
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} orb_vulkan_context;
