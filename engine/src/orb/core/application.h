#pragma once

#include "types.h"

#include "../platform/platform.h"

typedef struct application_config {
  i16 x;
  i16 y;
  i16 width;
  i16 height;

  const char* name;
} application_config;

typedef struct application_state {
  b8 is_initialized;
  b8 is_running;
  b8 is_suspended;
  platform_state platform;
  i16 width;
  i16 height;
  f64 last_tick;
} application_state;

ORB_API b8 orb_application_create(application_state *app,
                                  application_config *config);
ORB_API b8 orb_application_run(application_state *app);
