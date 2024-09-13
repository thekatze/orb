#include "vulkan_object_shader.h"
#include "../../../core/logger.h"
#include "../../../math/math_types.h"

#include "../vulkan_buffer.h"
#include "../vulkan_pipeline.h"
#include "../vulkan_shader_utils.h"

alignas(u32) const u8 vertex_shader_bin[] = {
#embed "orb-shaders/orb.builtin.object.vert.spv"
};

alignas(u32) const u8 fragment_shader_bin[] = {
#embed "orb-shaders/orb.builtin.object.frag.spv"
};

#define ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT 1U
#define ORB_OBJECT_SHADER_UNIFORM_ATTRIBUTE_COUNT 1U

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

    // Descriptors
    VkDescriptorSetLayoutBinding global_uniform_buffer_layout_binding = {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImmutableSamplers = VK_NULL_HANDLE,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutCreateInfo global_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &global_uniform_buffer_layout_binding,
    };

    ORB_VK_EXPECT(vkCreateDescriptorSetLayout(context->device.logical_device,
                                              &global_layout_create_info, context->allocator,
                                              &out_shader->global_descriptor_set_layout));

    VkDescriptorPoolSize global_descriptor_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = context->swapchain.image_count,
    };

    VkDescriptorPoolCreateInfo global_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &global_descriptor_pool_size,
        .maxSets = context->swapchain.image_count,
    };

    ORB_VK_EXPECT(vkCreateDescriptorPool(context->device.logical_device, &global_pool_create_info,
                                         context->allocator, &out_shader->global_descriptor_pool));

    // Descriptor set layouts
    VkDescriptorSetLayout layouts[ORB_OBJECT_SHADER_UNIFORM_ATTRIBUTE_COUNT] = {
        out_shader->global_descriptor_set_layout,
    };

    // Pipeline
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

    VkPipelineShaderStageCreateInfo stage_create_infos[ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT] = {};
    for (u32 i = 0; i < ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT; ++i) {
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }

    if (!orb_vulkan_graphics_pipeline_create(
            context, &context->main_renderpass, ORB_OBJECT_SHADER_VERTEX_ATTRIBUTE_COUNT,
            vertex_attribute_descriptions, ORB_OBJECT_SHADER_UNIFORM_ATTRIBUTE_COUNT, layouts,
            ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT, stage_create_infos, viewport, scissor, false,
            &out_shader->pipeline)) {
        ORB_ERROR("Failed to create graphics pipeline for object shader");
        return false;
    }

    if (!orb_vulkan_buffer_create(
            context, sizeof(orb_global_uniform_object) * context->swapchain.image_count,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &out_shader->global_uniform_buffer)) {
        ORB_ERROR("Failed to create buffer for object shader uniforms");
        return false;
    }

    orb_vulkan_buffer_bind(context, &out_shader->global_uniform_buffer, 0);

    VkDescriptorSetLayout global_layouts[ORB_MAX_IMAGE_BUFFERS] = {
        out_shader->global_descriptor_set_layout,
        out_shader->global_descriptor_set_layout,
        out_shader->global_descriptor_set_layout,
    };

    VkDescriptorSetAllocateInfo descriptor_allocation_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = out_shader->global_descriptor_pool,
        .descriptorSetCount = ORB_ARRAY_LENGTH(global_layouts),
        .pSetLayouts = global_layouts,
    };

    ORB_VK_EXPECT(vkAllocateDescriptorSets(context->device.logical_device,
                                           &descriptor_allocation_info,
                                           out_shader->global_descriptor_sets));

    return true;
}

void orb_vulkan_object_shader_destroy(orb_vulkan_context *context,
                                      orb_vulkan_object_shader *shader) {
    VkDevice logical_device = context->device.logical_device;

    orb_vulkan_buffer_destroy(context, &shader->global_uniform_buffer);

    orb_vulkan_graphics_pipeline_destroy(context, &shader->pipeline);

    vkDestroyDescriptorPool(logical_device, shader->global_descriptor_pool, context->allocator);
    vkDestroyDescriptorSetLayout(logical_device, shader->global_descriptor_set_layout,
                                 context->allocator);

    for (u32 i = 0; i < ORB_VULKAN_OBJECT_SHADER_STAGE_COUNT; ++i) {
        vkDestroyShaderModule(logical_device, shader->stages[i].handle, context->allocator);
        shader->stages[i].handle = 0;
    }

    *shader = (orb_vulkan_object_shader){0};
}

void orb_vulkan_object_shader_use(orb_vulkan_context *context, orb_vulkan_object_shader *shader) {
    orb_vulkan_graphics_pipeline_bind(&context->graphics_command_buffers[context->image_index],
                                      VK_PIPELINE_BIND_POINT_GRAPHICS, &shader->pipeline);
}

void orb_vulkan_object_shader_update_global_state(orb_vulkan_context *context,
                                                  orb_vulkan_object_shader *shader) {
    u32 image_index = context->image_index;
    VkCommandBuffer command_buffer = context->graphics_command_buffers[image_index].handle;
    VkDescriptorSet global_descriptor = shader->global_descriptor_sets[image_index];

    usize range = sizeof(orb_global_uniform_object);
    usize offset = sizeof(orb_global_uniform_object) * image_index;

    // upload object to buffer
    orb_vulkan_buffer_load_data_raw(context, &shader->global_uniform_buffer, offset, range, 0,
                                    &shader->global_uniform_object);

    VkDescriptorBufferInfo buffer_info = {
        .buffer = shader->global_uniform_buffer.handle,
        .offset = offset,
        .range = range,
    };

    VkWriteDescriptorSet descriptor_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = global_descriptor,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pBufferInfo = &buffer_info,
    };

    vkUpdateDescriptorSets(context->device.logical_device, 1, &descriptor_write, 0, VK_NULL_HANDLE);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            shader->pipeline.layout, 0, 1, &global_descriptor, 0, VK_NULL_HANDLE);
}

void orb_vulkan_object_shader_update_object(orb_vulkan_context *context, orb_vulkan_object_shader *shader,
                                     orb_mat4 model_transform) {
    u32 image_index = context->image_index;
    VkCommandBuffer command_buffer = context->graphics_command_buffers[image_index].handle;

    vkCmdPushConstants(command_buffer, shader->pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(orb_mat4), &model_transform);
}
