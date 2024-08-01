#include "vulkan_renderpass.h"

b8 orb_vulkan_renderpass_create(orb_vulkan_context *context,
                                orb_vulkan_renderpass *out_renderpass, //
                                i32 x, i32 y,                          // TODO: vec2 offset
                                u32 w, u32 h,                          // TODO: vec2 extents
                                f32 r, f32 g, f32 b,
                                f32 a, // TODO: vec4 clear color
                                f32 depth, u32 stencil) {

    out_renderpass->x = x;
    out_renderpass->y = y;
    out_renderpass->w = w;
    out_renderpass->h = h;
    out_renderpass->r = r;
    out_renderpass->g = g;
    out_renderpass->b = b;
    out_renderpass->a = a;
    out_renderpass->depth = depth;
    out_renderpass->stencil = stencil;

    const u32 attachment_description_count = 2;
    VkAttachmentDescription attachment_descriptions[attachment_description_count];

    const u32 color_attachment_index = 0;
    const u32 depth_attachment_index = 1;

    VkAttachmentDescription color_attachment = {
        .format = context->swapchain.image_format.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    attachment_descriptions[color_attachment_index] = color_attachment;

    VkAttachmentReference color_attachment_reference = {
        .attachment = color_attachment_index,
        // NOTE: VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL is used in tutorial, according
        // to spec its api 1.3 only,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentDescription depth_attachment = {
        .format = context->device.depth_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    attachment_descriptions[depth_attachment_index] = depth_attachment;

    VkAttachmentReference depth_attachment_reference = {
        .attachment = depth_attachment_index,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_reference,

        .pDepthStencilAttachment = &depth_attachment_reference,

        // input
        .inputAttachmentCount = 0,
        .pInputAttachments = 0,

        // multisampling colour attachments
        .pResolveAttachments = 0,

        // preserved for next subpass
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = 0,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = attachment_description_count,
        .pAttachments = attachment_descriptions,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    ORB_VK_EXPECT(vkCreateRenderPass(context->device.logical_device, &render_pass_info,
                                     context->allocator, &out_renderpass->handle));

    return true;
}

void orb_vulkan_renderpass_destroy(orb_vulkan_context *context, orb_vulkan_renderpass *renderpass) {
    if (renderpass && renderpass->handle) {
        vkDestroyRenderPass(context->device.logical_device, renderpass->handle, context->allocator);
        renderpass->handle = nullptr;
    }
}

void orb_vulkan_renderpass_begin(orb_vulkan_command_buffer *command_buffer,
                                 orb_vulkan_renderpass *renderpass, VkFramebuffer frame_buffer) {
    const u32 clear_value_count = 2;
    VkClearValue clear_values[clear_value_count];
    VkClearValue color = {
        .color =
            {
                .float32 = {renderpass->r, renderpass->g, renderpass->b, renderpass->a},
            },
    };
    clear_values[0] = color;

    VkClearValue depth = {
        .depthStencil =
            {
                .depth = renderpass->depth,
                .stencil = renderpass->stencil,
            },
    };
    clear_values[1] = depth;

    VkRenderPassBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderpass->handle,
        .framebuffer = frame_buffer,
        .clearValueCount = 2,
        .pClearValues = clear_values,
        .renderArea =
            {
                .offset =
                    {
                        .x = renderpass->x,
                        .y = renderpass->y,
                    },
                .extent =
                    {
                        .width = renderpass->w,
                        .height = renderpass->h,
                    },
            },
    };

    vkCmdBeginRenderPass(command_buffer->handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    command_buffer->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void orb_vulkan_renderpass_end(orb_vulkan_command_buffer *command_buffer,
                               orb_vulkan_renderpass *renderpass) {
    (void)renderpass;
    vkCmdEndRenderPass(command_buffer->handle);
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}
