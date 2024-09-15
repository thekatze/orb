#pragma once

#include <orb/game_types.h>
#include <orb/math/math_types.h>

typedef struct game_state {
    orb_mat4 camera_transform;
    orb_vec3 camera_position;
    orb_vec3 camera_euler;
    f32 camera_fov;
} game_state;

[[nodiscard]]
b8 initialize(orb_game *game);
void shutdown(orb_game *game);

[[nodiscard]]
b8 update(orb_game *game, f32 delta);

[[nodiscard]]
b8 render(orb_game *game, f32 delta);

void on_resize(orb_game *game, u16 width, u16 height);
