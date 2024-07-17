#pragma once

#include "renderer_types.h"

struct orb_static_mesh_data;
struct orb_platform_state;

b8 orb_renderer_init(struct orb_application_config* application_config,
                     struct orb_platform_state *platform_state);
void orb_renderer_shutdown();

void orb_renderer_resize(u16 width, u16 height);

b8 orb_renderer_draw_frame(orb_render_packet *packet);
