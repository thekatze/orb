#pragma once

#include "vulkan_types.h"

[[nodiscard]]
b8 orb_vulkan_command_buffer_allocate(orb_vulkan_context *context, VkCommandPool pool,
                                      b8 is_primary, orb_vulkan_command_buffer *out_command_buffer);

void orb_vulkan_command_buffer_free(orb_vulkan_context *context, VkCommandPool pool,
                                    orb_vulkan_command_buffer *out_command_buffer);

[[nodiscard]]
b8 orb_vulkan_command_buffer_begin(orb_vulkan_command_buffer *command_buffer,
                                   VkCommandBufferUsageFlags  flags);

[[nodiscard]]
b8 orb_vulkan_command_buffer_end(orb_vulkan_command_buffer *command_buffer);

// transitions
void orb_vulkan_command_buffer_update_submitted(orb_vulkan_command_buffer *command_buffer);
void orb_vulkan_command_buffer_reset(orb_vulkan_command_buffer *command_buffer);

// convenience for single use command buffers
[[nodiscard]]
b8 orb_vulkan_command_buffer_allocate_and_begin_single_use(
    orb_vulkan_context *context, VkCommandPool pool, orb_vulkan_command_buffer *out_command_buffer);

[[nodiscard]]
b8 orb_vulkan_command_buffer_end_and_submit_single_use(orb_vulkan_context *context,
                                                       VkCommandPool pool,
                                                       orb_vulkan_command_buffer *command_buffer,
                                                       VkQueue queue);
