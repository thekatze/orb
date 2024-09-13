#pragma once

#include "types.h"

// forward declare because of circular dependency
struct orb_game;

typedef struct orb_application_config {
    i16 x;
    i16 y;
    u16 width;
    u16 height;

    const char *name;
} orb_application_config;

[[nodiscard]]
ORB_API b8 orb_application_create(struct orb_game *game_instance);
[[nodiscard]]
ORB_API b8 orb_application_run();

ORB_API void orb_application_get_window_size(u16 *width, u16 *height);
