#include "vulkan_object_shader.h"
#include "../../../core/logger.h"

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

    return true;
}

void orb_vulkan_object_shader_destroy(orb_vulkan_context *context,
                                      orb_vulkan_object_shader *shader) {
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
