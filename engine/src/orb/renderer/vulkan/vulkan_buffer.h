#pragma once

#include "vulkan_types.h"

b8 orb_vulkan_buffer_create(orb_vulkan_context *context, usize size, VkBufferUsageFlagBits usage,
                            u32 memory_property_flags, orb_vulkan_buffer *out_buffer);

void orb_vulkan_buffer_destroy(orb_vulkan_context *context, orb_vulkan_buffer *buffer);

b8 orb_vulkan_buffer_bind(orb_vulkan_context *context, orb_vulkan_buffer *buffer, usize offset);

b8 orb_vulkan_buffer_resize(orb_vulkan_context *context, usize new_size, orb_vulkan_buffer *buffer,
                            VkQueue queue, VkCommandPool pool);

b8 orb_vulkan_buffer_memory_lock(orb_vulkan_context *context, orb_vulkan_buffer *buffer,
                                    usize offset, usize size, u32 flags, void* out_data);
void orb_vulkan_buffer_memory_unlock(orb_vulkan_context *context, orb_vulkan_buffer *buffer);

b8 orb_vulkan_buffer_load_data(orb_vulkan_context *context, orb_vulkan_buffer *buffer,
                                 usize offset, usize size, u32 flags, const void *data);

void orb_vulkan_buffer_memory_copy(orb_vulkan_context *context, VkCommandPool pool, VkFence fence,
                                   VkQueue queue, VkBuffer source, usize source_offset,
                                   VkBuffer destination, usize destination_offset, usize size);
