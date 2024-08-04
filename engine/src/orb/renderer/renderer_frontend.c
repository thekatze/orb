#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "../core/logger.h"
#include "../core/orb_memory.h"
#include "renderer_types.h"

static orb_renderer_backend *backend = nullptr;

b8 orb_renderer_init(struct orb_application_config *application_config) {
    backend = orb_allocate(sizeof(*backend), MEMORY_TAG_RENDERER);
    backend->frame_number = 0;

    orb_renderer_backend_init(backend);

    if (!backend->initialize(backend, application_config)) {
        ORB_FATAL("Renderer backend initialization failed.");
        return false;
    }

    return true;
}

void orb_renderer_shutdown() {
    if (backend) {
        backend->shutdown(backend);

        orb_free(backend, sizeof(*backend), MEMORY_TAG_RENDERER);
    }
}

void orb_renderer_resize(u16 width, u16 height) {
    // resize event can happen before the renderer has been initialized
    if (backend) {
        backend->resize(backend, width, height);
    }
}

b8 orb_renderer_draw_frame(orb_render_packet *packet) {
    // failing a begin frame might happen (after resizing), we can just try again
    // later
    if (!backend->begin_frame(backend, packet->delta_time)) {
        return true;
    }

    if (!backend->end_frame(backend, packet->delta_time)) {
        return false;
    }

    backend->frame_number += 1;

    return true;
}
