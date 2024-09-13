#pragma once

#include "renderer_types.h"

[[nodiscard]]
b8 orb_renderer_backend_init(orb_renderer_backend *out_renderer_backend);

void orb_renderer_backend_shutdown(orb_renderer_backend *renderer_backend);
