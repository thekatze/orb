#include "vulkan_command_buffer.h"
#include "vulkan_types.h"

b8 orb_vulkan_command_buffer_allocate(orb_vulkan_context *context, VkCommandPool pool,
                                      b8 is_primary,
                                      orb_vulkan_command_buffer *out_command_buffer) {
    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandBufferCount = 1,
    };

    out_command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;

    ORB_VK_EXPECT(vkAllocateCommandBuffers(context->device.logical_device, &allocate_info,
                                           &out_command_buffer->handle));

    out_command_buffer->state = COMMAND_BUFFER_STATE_READY;
    return true;
}

void orb_vulkan_command_buffer_free(orb_vulkan_context *context, VkCommandPool pool,
                                    orb_vulkan_command_buffer *out_command_buffer) {
    vkFreeCommandBuffers(context->device.logical_device, pool, 1, &out_command_buffer->handle);
    out_command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

b8 orb_vulkan_command_buffer_begin(orb_vulkan_command_buffer *command_buffer,
                                   orb_command_buffer_begin_flags flags) {

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags,
    };

    ORB_VK_EXPECT(vkBeginCommandBuffer(command_buffer->handle, &begin_info));
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;

    return true;
}

b8 orb_vulkan_command_buffer_end(orb_vulkan_command_buffer *command_buffer) {
    // TODO: ORB_DEBUG_ASSERT(expr, "command buffer must be in recording state")
    ORB_VK_EXPECT(vkEndCommandBuffer(command_buffer->handle));
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;

    return true;
}

// transitions

void orb_vulkan_command_buffer_update_submitted(orb_vulkan_command_buffer *command_buffer) {
    command_buffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void orb_vulkan_command_buffer_reset(orb_vulkan_command_buffer *command_buffer) {
    command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

// convenience for single use command buffers

b8 orb_vulkan_command_buffer_allocate_and_begin_single_use(
    orb_vulkan_context *context, VkCommandPool pool,
    orb_vulkan_command_buffer *out_command_buffer) {
    return orb_vulkan_command_buffer_allocate(context, pool, true, out_command_buffer) &&
           orb_vulkan_command_buffer_begin(out_command_buffer,
                                           VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

b8 orb_vulkan_command_buffer_end_and_submit_single_use(orb_vulkan_context *context,
                                                       VkCommandPool pool,
                                                       orb_vulkan_command_buffer *command_buffer,
                                                       VkQueue queue) {
    if (!orb_vulkan_command_buffer_end(command_buffer)) {
        return false;
    }

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer->handle,
    };

    ORB_VK_EXPECT(vkQueueSubmit(queue, 1, &submit_info, nullptr));

    // single use command buffers should not be used for big tasks, waiting is
    // acceptable here.
    ORB_VK_EXPECT(vkQueueWaitIdle(queue));

    orb_vulkan_command_buffer_free(context, pool, command_buffer);

    return true;
}
