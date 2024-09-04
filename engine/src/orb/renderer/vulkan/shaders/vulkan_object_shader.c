#include "vulkan_object_shader.h"
#include "../../../core/logger.h"
#include "../../../math/math_types.h"

#include "../vulkan_pipeline.h"
#include "../vulkan_shader_utils.h"

alignas(u32) const u8 vertex_shader_bin[] = {
#embed "orb-shaders/orb.builtin.object.vert.spv"
};

alignas(u32) const u8 fragment_shader_bin[] = {
#embed "orb-shaders/orb.builtin.object.frag.spv"
};

b8 orb_vulkan_object_shader_create(orb_vulkan_context *context,
                                   orb_vulkan_object_shader *out_shader) {

    if (!orb_create_shader_module(context, (const u32 *)vertex_shader_bin,
                                  ORB_ARRAY_LENGTH(vertex_shader_bin), VK_SHADER_STAGE_VERTEX_BIT,
                                  &out_shader->stages[0])) {
        ORB_ERROR("Unable to create builtin object vertex shader");
        return false;
    }

    if (!orb_create_shader_module(context, (const u32 *)fragment_shader_bin,
                                  ORB_ARRAY_LENGTH(fragment_shader_bin),
                                  VK_SHADER_STAGE_FRAGMENT_BIT, &out_shader->stages[1])) {
        ORB_ERROR("Unable to create builtin object fragment shader");
        return false;
    }

    // TODO: descriptors

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (f32)context->framebuffer_width,
        .height = (f32)context->framebuffer_height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent =
            {
                .width = context->framebuffer_width,
                .height = context->framebuffer_height,
            },
    };

#define ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT 1

    VkFormat formats[ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT] = {
        VK_FORMAT_R32G32B32_SFLOAT, // Position
    };

    u32 sizes[ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT] = {
        sizeof(orb_vec3), // Position
    };

    u32 offset = 0;
    VkVertexInputAttributeDescription
        vertex_attribute_descriptions[ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT];
    for (u32 i = 0; i < ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT; ++i) {
        vertex_attribute_descriptions[i].binding = 0;
        vertex_attribute_descriptions[i].location = i;
        vertex_attribute_descriptions[i].format = formats[i];
        vertex_attribute_descriptions[i].offset = offset;
        offset += sizes[i];
    }

    // TODO: descriptor set layouts

    VkPipelineShaderStageCreateInfo stage_create_infos[ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT] = {};
    for (u32 i = 0; i < ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT; ++i) {
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }

    if (!orb_vulkan_graphics_pipeline_create(
            context, &context->main_renderpass, ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT,
            vertex_attribute_descriptions, 0, nullptr, ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT,
            stage_create_infos, viewport, scissor, false, &out_shader->pipeline)) {
        ORB_ERROR("Failed to create graphics pipeline for object shader");
        return false;
    }

    ORB_DEBUG("Default object shader pipeline created");

    return true;
}

void orb_vulkan_object_shader_destroy(orb_vulkan_context *context,
                                      orb_vulkan_object_shader *shader) {
    orb_vulkan_graphics_pipeline_destroy(context, &shader->pipeline);

    for (u32 i = 0; i < ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT; ++i) {
        vkDestroyShaderModule(context->device.logical_device, shader->stages[i].handle,
                              context->allocator);
        shader->stages[i].handle = 0;
    }
}

void orb_vulkan_object_shader_use(orb_vulkan_context *context, orb_vulkan_object_shader *shader) {
    (void)context;
    (void)shader;
}
