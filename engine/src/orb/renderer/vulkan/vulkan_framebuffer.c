#include "vulkan_framebuffer.h"
#include "../../core/orb_memory.h"

b8 orb_vulkan_framebuffer_create(orb_vulkan_context *context,
                                 orb_vulkan_renderpass *renderpass, u32 width,
                                 u32 height, u32 attachment_count,
                                 VkImageView *attachments,
                                 orb_vulkan_framebuffer *out_framebuffer) {

  out_framebuffer->renderpass = renderpass;

  // copy attachments
  out_framebuffer->attachment_count = attachment_count;
  out_framebuffer->attachments =
      orb_allocate(sizeof(VkImageView) * attachment_count, MEMORY_TAG_RENDERER);

  orb_memory_copy(out_framebuffer->attachments, attachments,
                  sizeof(VkImageView) * attachment_count);

  VkFramebufferCreateInfo framebuffer_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = renderpass->handle,
      .attachmentCount = out_framebuffer->attachment_count,
      .pAttachments = out_framebuffer->attachments,
      .width = width,
      .height = height,
      .layers = 1,
  };

  ORB_VK_EXPECT(vkCreateFramebuffer(context->device.logical_device,
                                    &framebuffer_info, context->allocator,
                                    &out_framebuffer->handle));

  return TRUE;
}

void orb_vulkan_framebuffer_destroy(orb_vulkan_context *context,
                                    orb_vulkan_framebuffer *framebuffer) {
  vkDestroyFramebuffer(context->device.logical_device, framebuffer->handle,
                       context->allocator);
  if (framebuffer->attachments) {
    orb_free(framebuffer->attachments,
             sizeof(VkImageView) * framebuffer->attachment_count,
             MEMORY_TAG_RENDERER);
    framebuffer->attachments = 0;
    framebuffer->attachment_count = 0;
  }
}
