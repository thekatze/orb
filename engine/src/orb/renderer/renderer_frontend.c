#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "../core/expect.h"
#include "../math/orb_math.h"
#include "renderer_types.h"

typedef struct renderer_state {
    orb_renderer_backend backend;
} renderer_state;

static renderer_state *state;

b8 orb_renderer_init(usize *memory_requirement, void *memory,
                     struct orb_application_config *application_config) {
    *memory_requirement = sizeof(renderer_state);
    if (memory == nullptr) {
        return true;
    }

    state = (renderer_state *)memory;
    state->backend.frame_number = 0;

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
        state->backend.resize(&state->backend, width, height);
    }
}

b8 orb_renderer_draw_frame(orb_render_packet *packet) {
    // failing a begin frame might happen (after resizing), we can just try again
    // later
    if (!state->backend.begin_frame(&state->backend, packet->delta_time)) {
        return true;
    }

    orb_mat4 camera_projection =
        orb_mat4_perspective(orb_degrees_to_radians(50.0f), 1600.0f / 900.0f, 0.1f, 1000.0f);
    static f32 z = +3.0f;
    z += 2.0f * packet->delta_time;
    orb_mat4 camera_position = orb_mat4_from_translation((orb_vec3){.x = 0, .y = 0, .z = z});
    camera_position = orb_mat4_inverse(&camera_position);

    static f32 rotation = 0.0f;
    rotation += 30.0f * packet->delta_time;
    orb_mat4 object_transform = orb_mat4_identity();
    orb_mat4 rotation_matrix = orb_mat4_from_euler_rotation_y(orb_degrees_to_radians(rotation));
    object_transform = orb_mat4_mul(&rotation_matrix, &object_transform);

    ORB_EXPECT(state->backend.update_global_state(&(orb_global_uniform_object){
                   .camera_projection = orb_mat4_mul(&camera_position, &camera_projection),
               }),
               "failed uploading uniform");

    state->backend.update_object(object_transform);

    ORB_EXPECT(state->backend.end_frame(&state->backend, packet->delta_time),
               "Submitting frame failed");

    state->backend.frame_number += 1;

    return true;
}
