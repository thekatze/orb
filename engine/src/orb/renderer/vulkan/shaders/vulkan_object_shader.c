#include "vulkan_object_shader.h"
#include "../../../core/logger.h"

#include "../vulkan_shader_utils.h"

// HACK: this is highly temporary, find out how to better refer to assets
#define ORB_BUILTIN_OBJECT_VERTEX_SHADER_PATH "./build/engine/orb.builtin.object.vert.spv"
#define ORB_BUILTIN_OBJECT_FRAGMENT_SHADER_PATH "./build/engine/orb.builtin.object.frag.spv"

b8 orb_vulkan_object_shader_create(orb_vulkan_context *context,
                                   orb_vulkan_object_shader *out_shader) {
    if (!orb_create_shader_module(context, ORB_BUILTIN_OBJECT_VERTEX_SHADER_PATH,
                                  VK_SHADER_STAGE_VERTEX_BIT, &out_shader->stages[0])) {
        ORB_ERROR("Unable to create builtin object vertex shader");
        return false;
    }

    if (!orb_create_shader_module(context, ORB_BUILTIN_OBJECT_FRAGMENT_SHADER_PATH,
                                  VK_SHADER_STAGE_FRAGMENT_BIT, &out_shader->stages[1])) {
        ORB_ERROR("Unable to create builtin object fragment shader");
        return false;
    }

    // TODO: descriptors

    return true;
}

void orb_vulkan_object_shader_destroy(orb_vulkan_context *context,
                                      orb_vulkan_object_shader *shader) {
    (void)context;
    (void)shader;
}

void orb_vulkan_object_shader_use(orb_vulkan_context *context, orb_vulkan_object_shader *shader) {
    (void)context;
    (void)shader;
}
