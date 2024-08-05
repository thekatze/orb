#pragma once

#include "../../renderer_types.h"
#include "../vulkan_types.h"

b8 orb_vulkan_object_shader_create(orb_vulkan_context* context, orb_vulkan_object_shader* out_shader);
void orb_vulkan_object_shader_destroy(orb_vulkan_context* context, orb_vulkan_object_shader* shader);

void orb_vulkan_object_shader_use(orb_vulkan_context* context, orb_vulkan_object_shader* shader);
