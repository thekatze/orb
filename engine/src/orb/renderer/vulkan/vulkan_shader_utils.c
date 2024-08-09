#include "vulkan_shader_utils.h"

#include "../../core/orb_memory.h"

#define ORB_DEFERRED(cleanup_fn) __attribute((__cleanup__(cleanup_fn)))

b8 orb_create_shader_module(orb_vulkan_context *context, const u32 *shader_bytes,
                            usize shader_bytes_length, VkShaderStageFlagBits shader_stage,
                            orb_vulkan_shader_stage *out_shader_stage) {
    orb_memory_zero(out_shader_stage, sizeof(orb_vulkan_shader_stage));

    out_shader_stage->module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    out_shader_stage->module_create_info.codeSize = shader_bytes_length;
    out_shader_stage->module_create_info.pCode = shader_bytes;

    ORB_VK_EXPECT(vkCreateShaderModule(context->device.logical_device,
                                       &out_shader_stage->module_create_info, context->allocator,
                                       &out_shader_stage->handle));

    out_shader_stage->shader_stage_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    out_shader_stage->shader_stage_create_info.stage = shader_stage;
    out_shader_stage->shader_stage_create_info.module = out_shader_stage->handle;
    out_shader_stage->shader_stage_create_info.pName = "main"; // entry point to shader

    return true;
}
