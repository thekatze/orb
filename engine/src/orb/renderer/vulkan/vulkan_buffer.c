#include "vulkan_buffer.h"

#include "vulkan_command_buffer.h"
#include "vulkan_device.h"

#include "../../core/asserts.h"
#include "../../core/logger.h"
#include "../../core/orb_memory.h"

#include "vulkan_types.h"

b8 orb_vulkan_buffer_create(orb_vulkan_context *context, usize size, VkBufferUsageFlagBits usage,
                            u32 memory_property_flags, orb_vulkan_buffer *out_buffer) {
    *out_buffer = (orb_vulkan_buffer){
        .total_size = size, .usage = usage, .memory_property_flags = memory_property_flags};

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    ORB_VK_EXPECT(vkCreateBuffer(context->device.logical_device, &buffer_info, context->allocator,
                                 &out_buffer->handle));

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logical_device, out_buffer->handle,
                                  &requirements);

    out_buffer->memory_index = orb_vulkan_find_memory_index(requirements.memoryTypeBits,
                                                            out_buffer->memory_property_flags);
    if (out_buffer->memory_index == ORB_INVALID_INDEX) {
        ORB_ERROR(
            "Unable to create vulkan buffer because the required memory type index was not found");
        return false;
    }

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = out_buffer->memory_index,
    };

    ORB_VK_EXPECT(vkAllocateMemory(context->device.logical_device, &allocate_info,
                                   context->allocator, &out_buffer->memory));

    return true;
}

void orb_vulkan_buffer_destroy(orb_vulkan_context *context, orb_vulkan_buffer *buffer) {
    if (buffer->memory) {
        vkFreeMemory(context->device.logical_device, buffer->memory, context->allocator);
    }
    if (buffer->handle) {
        vkDestroyBuffer(context->device.logical_device, buffer->handle, context->allocator);
    }

    *buffer = (orb_vulkan_buffer){0};
}

b8 orb_vulkan_buffer_bind(orb_vulkan_context *context, orb_vulkan_buffer *buffer, usize offset) {
    ORB_VK_EXPECT(
        vkBindBufferMemory(context->device.logical_device, buffer->handle, buffer->memory, offset));

    return true;
}

b8 orb_vulkan_buffer_resize(orb_vulkan_context *context, usize new_size, orb_vulkan_buffer *buffer,
                            VkQueue queue, VkCommandPool pool) {
    ORB_ASSERT(buffer->handle != nullptr || buffer->memory != nullptr,
               "can not resize uninitialized buffer");

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = new_size,
        .usage = buffer->usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VkBuffer new_buffer;
    ORB_VK_EXPECT(vkCreateBuffer(context->device.logical_device, &buffer_info, context->allocator,
                                 &new_buffer));

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logical_device, new_buffer, &requirements);

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = buffer->memory_index,
    };

    VkDeviceMemory new_memory;
    ORB_VK_EXPECT(vkAllocateMemory(context->device.logical_device, &allocate_info,
                                   context->allocator, &new_memory));

    ORB_VK_EXPECT(vkBindBufferMemory(context->device.logical_device, new_buffer, new_memory, 0));

    orb_vulkan_buffer_memory_copy(context, pool, 0, queue, buffer->handle, 0, new_buffer, 0,
                                  buffer->total_size);

    vkDeviceWaitIdle(context->device.logical_device);

    vkFreeMemory(context->device.logical_device, buffer->memory, context->allocator);
    vkDestroyBuffer(context->device.logical_device, buffer->handle, context->allocator);

    buffer->memory = new_memory;
    buffer->handle = new_buffer;
    buffer->total_size = new_size;

    return true;
}

b8 orb_vulkan_buffer_memory_lock(orb_vulkan_context *context, orb_vulkan_buffer *buffer,
                                 usize offset, usize size, u32 flags, void *out_data) {
    ORB_VK_EXPECT(vkMapMemory(context->device.logical_device, buffer->memory, offset, size, flags,
                              &out_data));

    return true;
}

void orb_vulkan_buffer_memory_unlock(orb_vulkan_context *context, orb_vulkan_buffer *buffer) {
    vkUnmapMemory(context->device.logical_device, buffer->memory);
}

b8 orb_vulkan_buffer_load_data(orb_vulkan_context *context, orb_vulkan_buffer *buffer, usize offset,
                               usize size, u32 flags, const void *data) {
    void *destination;
    ORB_VK_EXPECT(vkMapMemory(context->device.logical_device, buffer->memory, offset, size, flags,
                              &destination));

    orb_memory_copy(destination, data, size);

    vkUnmapMemory(context->device.logical_device, buffer->memory);

    return true;
}

void orb_vulkan_buffer_memory_copy(orb_vulkan_context *context, VkCommandPool pool, VkFence fence,
                                   VkQueue queue, VkBuffer source, usize source_offset,
                                   VkBuffer destination, usize destination_offset, usize size) {
    (void)fence;

    vkQueueWaitIdle(queue);

    orb_vulkan_command_buffer copy_command_buffer;
    orb_vulkan_command_buffer_allocate_and_begin_single_use(context, pool, &copy_command_buffer);

    VkBufferCopy copy_region = {
        .srcOffset = source_offset,
        .dstOffset = destination_offset,
        .size = size,
    };

    vkCmdCopyBuffer(copy_command_buffer.handle, source, destination, 1, &copy_region);

    orb_vulkan_command_buffer_end_and_submit_single_use(context, pool, &copy_command_buffer, queue);
}
