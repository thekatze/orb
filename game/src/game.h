#pragma once

#include <orb/game_types.h>
#include <orb/math/random.h>

typedef struct game_state {
    f32 delta_time;
    orb_rng_state rng;
} game_state;

[[nodiscard]]
b8 initialize(orb_game *game);
void shutdown(orb_game *game);

[[nodiscard]]
b8 update(orb_game *game, f32 delta);

[[nodiscard]]
b8 render(orb_game *game, f32 delta);

void on_resize(orb_game *game, u16 width, u16 height);
