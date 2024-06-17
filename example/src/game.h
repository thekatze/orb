#pragma once

#include <orb/game_types.h>

typedef struct game_state {
  f32 delta_time;
} game_state;

b8 initialize(game *game);

b8 update(game *game, float delta);

b8 render(game *game, float delta);

void on_resize(game *game, u16 width, u16 height);
