#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 orb_renderer_backend_init(orb_renderer_backend *out_renderer_backend) {
    out_renderer_backend->initialize = vulkan_backend_initialize;
    out_renderer_backend->shutdown = vulkan_backend_shutdown;
    out_renderer_backend->begin_frame = vulkan_backend_begin_frame;
    out_renderer_backend->end_frame = vulkan_backend_end_frame;
    out_renderer_backend->resize = vulkan_backend_resize;

    return true;
}

void orb_renderer_backend_shutdown(orb_renderer_backend *renderer_backend) {
    renderer_backend->initialize = nullptr;
    renderer_backend->shutdown = nullptr;
    renderer_backend->resize = nullptr;
    renderer_backend->begin_frame = nullptr;
    renderer_backend->end_frame = nullptr;
}
