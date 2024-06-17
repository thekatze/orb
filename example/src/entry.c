#include "game.h"
#include <orb/core/orb_memory.h>
#include <orb/entry.h>

b8 create_game(game *out_game) {
  out_game->app_config.x = 100;
  out_game->app_config.y = 100;
  out_game->app_config.width = 1600;
  out_game->app_config.height = 900;
  out_game->app_config.name = "orb engine";

  out_game->initialize = initialize;
  out_game->update = update;
  out_game->render = render;
  out_game->on_resize = on_resize;

  game_state *state =
      (game_state *)orb_allocate(sizeof(game_state), MEMORY_TAG_GAME);

  state->delta_time = 0.016f;
  out_game->state = state;

  return TRUE;
}
