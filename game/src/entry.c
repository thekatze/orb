#include "game.h"
#include <orb/core/orb_memory.h>
#include <orb/entry.h>
#include <orb/math/orb_math.h>

b8 create_game(orb_game *out_game) {
    game_state *state = (game_state *)orb_allocate(sizeof(game_state), MEMORY_TAG_GAME);

    *state = (game_state){
        .camera_transform = orb_mat4_from_translation((orb_vec3){.z = 10.0f}),
        .camera_fov = 80.0f,
        .camera_position = {.z = 10.0f}
    };

    *out_game = (orb_game){
        .app_config =
            {
                .x = 100,
                .y = 100,
                .width = 1600,
                .height = 900,
                .name = "orb engine",
            },
        .state = state,

        .initialize = initialize,
        .shutdown = shutdown,
        .update = update,
        .render = render,
        .on_resize = on_resize,

    };

    return true;
}
