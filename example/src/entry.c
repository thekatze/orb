#include "game.h"
#include <orb/entry.h>

// TODO: we dont want platform specific code here
#include <orb/platform/platform.h>

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

  out_game->state =
      (game_state *)orb_platform_allocate(sizeof(game_state), FALSE);

  return TRUE;
}
