#pragma once

#include "renderer_types.h"

struct orb_platform_state;

b8 orb_renderer_backend_init(struct orb_platform_state *platform_state,
                             orb_renderer_backend *out_renderer_backend);

void orb_renderer_backend_shutdown(orb_renderer_backend *renderer_backend);
