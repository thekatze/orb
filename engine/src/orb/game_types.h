#pragma once

#include "core/application.h"
#include "core/types.h"

typedef struct orb_game {
    orb_application_config app_config;

    b8 (*initialize)(struct orb_game *game_instance);
    b8 (*update)(struct orb_game *game_instance, f32 delta_time);
    b8 (*render)(struct orb_game *game_instance, f32 delta_time);
    void (*on_resize)(struct orb_game *game_instance, u16 width, u16 height);

    void (*shutdown)(struct orb_game *game_instance);

    // game specific state managed by the game
    void *state;

    // application state managed by the engine
    void *application_state;
} orb_game;
