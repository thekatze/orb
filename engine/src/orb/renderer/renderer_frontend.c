#include "renderer_frontend.h"

#include "renderer_backend.h"
#include "renderer_types.h"

#include "../core/application.h"
#include "../core/expect.h"
#include "../math/orb_math.h"

typedef struct orb_camera {
    orb_mat4 inverse_transform;
    f32 fov;
    f32 near_clip;
    f32 far_clip;
} orb_camera;

typedef struct renderer_state {
    orb_renderer_backend backend;
    orb_camera camera;
    orb_mat4 camera_projection;
    u16 width;
    u16 height;
} renderer_state;

static renderer_state *state;

b8 orb_renderer_init(usize *memory_requirement, void *memory,
                     orb_application_config *application_config) {
    *memory_requirement = sizeof(renderer_state);
    if (memory == nullptr) {
        return true;
    }

    state = (renderer_state *)memory;
    *state = (renderer_state){0};

    state->width = application_config->width;
    state->height = application_config->height;

    state->camera = (orb_camera){
        .inverse_transform = orb_mat4_identity(),
        .fov = 80.0f,
        .near_clip = 0.01f,
        .far_clip = 1000.0f,
    };

    orb_renderer_set_camera(&state->camera.inverse_transform, state->camera.fov,
                            state->camera.near_clip, state->camera.far_clip);

    ORB_EXPECT(orb_renderer_backend_init(&state->backend) &&
                   state->backend.initialize(&state->backend, application_config),
               "Renderer backend initialization failed");

    return true;
}

void orb_renderer_shutdown() {
    if (state) {
        state->backend.shutdown(&state->backend);
    }
}

void orb_renderer_resize(u16 width, u16 height) {
    // resize event can happen before the renderer has been initialized
    if (state) {
        state->width = width;
        state->height = height;
        orb_renderer_set_camera(&state->camera.inverse_transform, state->camera.fov,
                                state->camera.near_clip, state->camera.far_clip);

        state->backend.resize(&state->backend, width, height);
    }
}

void orb_renderer_set_camera(const orb_mat4 *inverse_transform, f32 fov, f32 near_clip,
                             f32 far_clip) {
    state->camera = (orb_camera){
        .inverse_transform = *inverse_transform,
        .fov = fov,
        .near_clip = near_clip,
        .far_clip = far_clip,
    };

    orb_mat4 projection;
    if (fov == 0.0f) {
        projection = orb_mat4_orthographic(0, state->width, 0, state->height, near_clip, far_clip);
    } else {
        f32 aspect_ratio = (f32)state->width / (f32)state->height;
        projection = orb_mat4_perspective(fov, aspect_ratio, near_clip, far_clip);
    }

    state->camera_projection = orb_mat4_mul(inverse_transform, &projection);
}

b8 orb_renderer_draw_frame(orb_render_packet *packet) {
    // failing a begin frame might happen (after resizing), we can just try again
    // later
    if (!state->backend.begin_frame(&state->backend, packet->delta_time)) {
        return true;
    }

    static f32 rotation = 0.0f;
    rotation += 30.0f * packet->delta_time;
    orb_mat4 object_transform = orb_mat4_identity();
    orb_mat4 rotation_matrix = orb_mat4_from_euler_rotation_z(orb_degrees_to_radians(rotation));
    object_transform = orb_mat4_mul(&rotation_matrix, &object_transform);

    ORB_EXPECT(state->backend.update_global_state(&(orb_global_uniform_object){
                   .camera_projection = state->camera_projection,
               }),
               "failed uploading uniform");

    state->backend.update_object(object_transform);

    ORB_EXPECT(state->backend.end_frame(&state->backend, packet->delta_time),
               "Submitting frame failed");

    state->backend.frame_number += 1;

    return true;
}
