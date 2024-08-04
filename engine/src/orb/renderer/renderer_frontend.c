#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "../core/logger.h"
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

    orb_renderer_backend_init(&state->backend);

    if (!state->backend.initialize(&state->backend, application_config)) {
        ORB_FATAL("Renderer backend initialization failed.");
        return false;
    }

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

    if (!state->backend.end_frame(&state->backend, packet->delta_time)) {
        return false;
    }

    state->backend.frame_number += 1;

    return true;
}
