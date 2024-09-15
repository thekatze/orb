#pragma once

#include "renderer_types.h"

struct orb_static_mesh_data;

[[nodiscard]]
b8 orb_renderer_init(usize *memory_requirement, void *memory,
                     struct orb_application_config *application_config);

void orb_renderer_shutdown();

void orb_renderer_resize(u16 width, u16 height);

// HACK: temporarily public api
ORB_API void orb_renderer_set_camera(const orb_mat4 *transform, f32 fov, f32 near_clip, f32 far_clip);

[[nodiscard]]
b8 orb_renderer_draw_frame(orb_render_packet *packet);
