#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "../core/logger.h"
#include "../core/orb_memory.h"
#include "renderer_types.h"

static orb_renderer_backend *backend = nullptr;

b8 orb_renderer_init(struct orb_application_config *application_config,
                     struct orb_platform_state *platform_state) {
  backend = orb_allocate(sizeof(*backend), MEMORY_TAG_RENDERER);
  backend->frame_number = 0;

  orb_renderer_backend_init(platform_state, backend);

  if (!backend->initialize(backend, application_config, platform_state)) {
    ORB_FATAL("Renderer backend initialization failed.");
    return FALSE;
  }

  return TRUE;
}

void orb_renderer_shutdown() {
  if (backend) {
    backend->shutdown(backend);

    orb_free(backend, sizeof(*backend), MEMORY_TAG_RENDERER);
  }
}

void orb_renderer_resize(u16 width, u16 height) {
  backend->resize(backend, width, height);
}

b8 orb_renderer_draw_frame(orb_render_packet *packet) {
  // failing a begin frame might happen (after resizing), we can just try again
  // later
  if (!backend->begin_frame(backend, packet->delta_time)) {
    return TRUE;
  }

  if (!backend->end_frame(backend, packet->delta_time)) {
    return FALSE;
  }

  backend->frame_number += 1;

  return TRUE;
}
