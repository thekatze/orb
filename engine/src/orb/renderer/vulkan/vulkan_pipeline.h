#pragma once

#include "vulkan_types.h"

[[nodiscard]]
b8 orb_vulkan_graphics_pipeline_create(orb_vulkan_context *context,
                                       orb_vulkan_renderpass *renderpass, u32 attribute_count,
                                       VkVertexInputAttributeDescription *attributes,
                                       u32 descriptor_set_layout_count,
                                       VkDescriptorSetLayout *descriptor_set_layouts,
                                       u32 stage_count, VkPipelineShaderStageCreateInfo *stages,
                                       VkViewport viewport, VkRect2D scissor, b8 is_wireframe,
                                       orb_vulkan_pipeline *out_pipeline);

void orb_vulkan_graphics_pipeline_destroy(orb_vulkan_context *context,
                                          orb_vulkan_pipeline *pipeline);

void orb_vulkan_graphics_pipeline_bind(orb_vulkan_command_buffer *command_buffer,
                                       VkPipelineBindPoint bind_point,
                                       orb_vulkan_pipeline *pipeline);
