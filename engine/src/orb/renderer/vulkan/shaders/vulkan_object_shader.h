#pragma once

#include "../vulkan_types.h"

[[nodiscard]]
b8 orb_vulkan_object_shader_create(orb_vulkan_context *context,
                                   orb_vulkan_object_shader *out_shader);
void orb_vulkan_object_shader_destroy(orb_vulkan_context *context,
                                      orb_vulkan_object_shader *shader);

void orb_vulkan_object_shader_use(orb_vulkan_context *context, orb_vulkan_object_shader *shader);

[[nodiscard]]
b8 orb_vulkan_object_shader_update_global_state(orb_vulkan_context *context,
                                                  orb_vulkan_object_shader *shader);

void orb_vulkan_object_shader_update_object(orb_vulkan_context *context, orb_vulkan_object_shader *shader,
                                     orb_mat4 model_transform);
