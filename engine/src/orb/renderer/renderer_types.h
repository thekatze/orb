#pragma once

#include "../core/types.h"
#include "../math/math_types.h"

struct orb_application_config;

typedef struct orb_global_uniform_object {
    // projection * view
    orb_mat4 camera_projection; // 16 * float (4 bytes) = 64 bytes

    // NOTE: according to the kohi series, nvida cards demand 256 bytes of uniform object. validate
    // this claim.
    u8 _padding[256 - 64];
} orb_global_uniform_object;

typedef struct orb_renderer_backend {
    u32 frame_number;

    b8 (*initialize)(struct orb_renderer_backend *backend,
                     struct orb_application_config *application_config);

    void (*shutdown)(struct orb_renderer_backend *backend);

    void (*resize)(struct orb_renderer_backend *backend, u16 width, u16 height);

    b8 (*begin_frame)(struct orb_renderer_backend *backend, f32 delta_time);
    void (*update_global_state)(const orb_global_uniform_object* global_state);
    b8 (*end_frame)(struct orb_renderer_backend *backend, f32 delta_time);

    void (*update_object)(orb_mat4 model);
} orb_renderer_backend;

typedef struct orb_render_packet {
    f32 delta_time;
} orb_render_packet;
