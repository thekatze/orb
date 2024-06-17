#pragma once

#include "core/types.h"
#include "core/application.h"

typedef struct game {
  application_config app_config;

  b8 (*initialize)(struct game *game_instance);
  b8 (*update)(struct game *game_instance, f32 delta_time);
  b8 (*render)(struct game *game_instance, f32 delta_time);
  void (*on_resize)(struct game *game_instance, u16 width, u16 height);

  // game specific state managed by the game
  void *state;
} game;
