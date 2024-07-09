#pragma once

#include "../core/types.h"

typedef struct orb_renderer_backend {
  struct orb_platform_state *platform_state;
  u32 frame_number;

  b8 (*initialize)(struct orb_renderer_backend *backend,
                   const char *application_name,
                   struct orb_platform_state *platform_state);

  void (*shutdown)(struct orb_renderer_backend *backend);

  void (*resize)(struct orb_renderer_backend *backend, u16 width, u16 height);

  b8 (*begin_frame)(struct orb_renderer_backend *backend, f32 delta_time);
  b8 (*end_frame)(struct orb_renderer_backend *backend, f32 delta_time);
} orb_renderer_backend;

typedef struct orb_render_packet {
  f32 delta_time;
} orb_render_packet;
