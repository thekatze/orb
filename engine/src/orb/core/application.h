#pragma once

#include "types.h"

// forward declare because of circular dependency
struct game;

typedef struct application_config {
  i16 x;
  i16 y;
  u16 width;
  u16 height;

  const char *name;
} application_config;


ORB_API b8 orb_application_create(struct game* game_instance);
ORB_API b8 orb_application_run();
