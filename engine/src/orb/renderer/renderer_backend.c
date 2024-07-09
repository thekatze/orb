#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 orb_renderer_backend_init(struct orb_platform_state *platform_state,
                             orb_renderer_backend *out_renderer_backend) {
  out_renderer_backend->platform_state = platform_state;

  out_renderer_backend->initialize = vulkan_backend_initialize;
  out_renderer_backend->shutdown = vulkan_backend_shutdown;
  out_renderer_backend->begin_frame = vulkan_backend_begin_frame;
  out_renderer_backend->end_frame = vulkan_backend_end_frame;
  out_renderer_backend->resize = vulkan_backend_resize;

  return TRUE;
}

void orb_renderer_backend_shutdown(orb_renderer_backend *renderer_backend) {
  renderer_backend->initialize = 0;
  renderer_backend->shutdown = 0;
  renderer_backend->resize = 0;
  renderer_backend->begin_frame = 0;
  renderer_backend->end_frame = 0;
}
