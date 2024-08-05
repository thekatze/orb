#pragma once

#include "vulkan_types.h"

b8 orb_create_shader_module(orb_vulkan_context* context, const char* name, VkShaderStageFlagBits shader_stage, orb_vulkan_shader_stage* out_shader_stage);

