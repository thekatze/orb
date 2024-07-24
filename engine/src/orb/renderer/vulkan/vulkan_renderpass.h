#pragma once

#include "vulkan_types.h"

b8 orb_vulkan_renderpass_create(orb_vulkan_context *context,
                                orb_vulkan_renderpass *out_renderpass, //
                                i32 x, i32 y,                          // TODO: vec2 offset
                                u32 w, u32 h,                          // TODO: vec2 extents
                                f32 r, f32 g, f32 b,
                                f32 a, // TODO: vec4 clear color
                                f32 depth, u32 stencil);

void orb_vulkan_renderpass_destroy(orb_vulkan_context *context, orb_vulkan_renderpass *renderpass);

void orb_vulkan_renderpass_begin(orb_vulkan_command_buffer *command_buffer,
                                 orb_vulkan_renderpass *renderpass, VkFramebuffer frame_buffer);

void orb_vulkan_renderpass_end(orb_vulkan_command_buffer *command_buffer,
                               orb_vulkan_renderpass *renderpass);
