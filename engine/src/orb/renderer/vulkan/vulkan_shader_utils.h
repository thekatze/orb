#pragma once

#include "vulkan_types.h"

b8 orb_create_shader_module(orb_vulkan_context *context, const u32 *shader_bytes,
                            usize shader_bytes_length, VkShaderStageFlagBits shader_stage,
                            orb_vulkan_shader_stage *out_shader_stage);

