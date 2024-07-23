#pragma once

#include <orb/game_types.h>

typedef struct game_state {
  f32 delta_time;
} game_state;

b8 initialize(orb_game *game);
void shutdown(orb_game *game);

b8 update(orb_game *game, float delta);

b8 render(orb_game *game, float delta);

void on_resize(orb_game *game, u16 width, u16 height);
