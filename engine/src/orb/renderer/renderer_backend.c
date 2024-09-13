#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 orb_renderer_backend_init(orb_renderer_backend *out_renderer_backend) {
    out_renderer_backend->initialize = orb_vulkan_backend_initialize;
    out_renderer_backend->shutdown = orb_vulkan_backend_shutdown;
    out_renderer_backend->begin_frame = orb_vulkan_backend_begin_frame;
    out_renderer_backend->update_global_state = orb_vulkan_backend_update_global_state;
    out_renderer_backend->end_frame = orb_vulkan_backend_end_frame;
    out_renderer_backend->resize = orb_vulkan_backend_resize;
    out_renderer_backend->update_object = orb_vulkan_backend_update_object;

    return true;
}

void orb_renderer_backend_shutdown(orb_renderer_backend *renderer_backend) {
    *renderer_backend = (orb_renderer_backend){0};
}
