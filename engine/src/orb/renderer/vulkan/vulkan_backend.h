#pragma once

#include "../renderer_backend.h"

b8 orb_vulkan_backend_initialize(orb_renderer_backend *backend,
                                 struct orb_application_config *application_config);

void orb_vulkan_backend_shutdown(orb_renderer_backend *backend);

void orb_vulkan_backend_resize(orb_renderer_backend *backend, u16 width, u16 height);

b8 orb_vulkan_backend_begin_frame(orb_renderer_backend *backend, f32 delta_time);
void orb_vulkan_backend_update_global_state(const orb_global_uniform_object *global_state);
b8 orb_vulkan_backend_end_frame(orb_renderer_backend *backend, f32 delta_time);
