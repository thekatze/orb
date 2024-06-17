#include "game.h"
#include "orb/core/logger.h"

b8 initialize(game *game_instance) {
  ORB_TRACE("INIT");
  (void)game_instance;
  return TRUE;
}

b8 update(game *game_instance, float delta) {
  ORB_TRACE("UPDATE");
  (void)game_instance;
  (void)delta;
  return TRUE;
}

b8 render(game *game_instance, float delta) {
  ORB_TRACE("RENDER");
  (void)game_instance;
  (void)delta;
  return TRUE;
}

void on_resize(game *game_instance, u16 width, u16 height) {
  (void)game_instance;
  (void)width;
  (void)height;
}
