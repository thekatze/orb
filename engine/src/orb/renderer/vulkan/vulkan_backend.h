#pragma once

#include "../renderer_backend.h"

b8 vulkan_backend_initialize(orb_renderer_backend *backend,
                             struct orb_application_config *application_config);

void vulkan_backend_shutdown(orb_renderer_backend *backend);

void vulkan_backend_resize(orb_renderer_backend *backend, u16 width, u16 height);

b8 vulkan_backend_begin_frame(orb_renderer_backend *backend, f32 delta_time);
b8 vulkan_backend_end_frame(orb_renderer_backend *backend, f32 delta_time);
