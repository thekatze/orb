#include "vulkan_shader_utils.h"

#include "../../core/logger.h"
#include "../../core/orb_memory.h"
#include "../../platform/filesystem.h"

#define ORB_DEFERRED(cleanup_fn) __attribute((__cleanup__(cleanup_fn)))

b8 orb_create_shader_module(orb_vulkan_context *context, const char *path,
                            VkShaderStageFlagBits shader_stage,
                            orb_vulkan_shader_stage *out_shader_stage) {
    orb_memory_zero(out_shader_stage, sizeof(orb_vulkan_shader_stage));

    out_shader_stage->module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    orb_file_handle shader_file;
    if (!orb_filesystem_file_open(path, FILE_MODE_READ_FLAG, true, &shader_file)) {
        ORB_ERROR("Unable to read shader module: %s", path);
        return false;
    }

    u32 *bytes = orb_allocate(shader_file.file_bytes, MEMORY_TAG_RENDERER);

    if (!orb_filesystem_file_read_all_bytes(&shader_file, bytes)) {
        ORB_ERROR("Unable to read shader module as binary file: %s", path);

        orb_filesystem_file_close(&shader_file);
        orb_free(bytes, shader_file.file_bytes, MEMORY_TAG_RENDERER);

        return false;
    }

    orb_filesystem_file_close(&shader_file);

    out_shader_stage->module_create_info.codeSize = shader_file.file_bytes;
    out_shader_stage->module_create_info.pCode = bytes;

    ORB_VK_EXPECT(vkCreateShaderModule(context->device.logical_device,
                                       &out_shader_stage->module_create_info, context->allocator,
                                       &out_shader_stage->handle));

    out_shader_stage->shader_stage_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    out_shader_stage->shader_stage_create_info.stage = shader_stage;
    out_shader_stage->shader_stage_create_info.module = out_shader_stage->handle;
    out_shader_stage->shader_stage_create_info.pName = "main"; // entry point to shader

    orb_free(bytes, shader_file.file_bytes, MEMORY_TAG_RENDERER);

    return true;
}
