#include "game.h"
#include "orb/core/event.h"
#include "orb/core/input.h"
#include "orb/core/logger.h"

b8 handle_key_press(u16 code, void *listener, void *source,
                    orb_event_context context) {
  (void)code;
  (void)listener;
  (void)source;
  switch (context.data.u16[0]) {
  case KEY_A:
    ORB_INFO("A PRESSED!");
    break;
  }

  return FALSE;
}

b8 handle_mouse_button(u16 code, void *listener, void *source,
                       orb_event_context context) {
  (void)code;
  (void)listener;
  (void)source;
  switch (context.data.u16[0]) {
  case MOUSE_BUTTON_LEFT:
    ORB_INFO("LEFT MOUSE BUTTON CLICKED!");
    break;
  }

  return FALSE;
}

b8 handle_mouse_move(u16 code, void *listener, void *source,
                     orb_event_context context) {
  (void)code;
  (void)listener;
  (void)source;
  i16 x = context.data.i16[0];
  i16 y = context.data.i16[1];

  ORB_INFO("Mouse moved: (%i, %i)", x, y);

  return FALSE;
}

b8 initialize(game *game_instance) {
  (void)game_instance;

  orb_event_add_listener(ORB_EVENT_KEY_PRESSED, game_instance,
                         handle_key_press);
  orb_event_add_listener(ORB_EVENT_MOUSE_BUTTON_PRESSED, game_instance,
                         handle_mouse_button);

  return TRUE;
}

b8 update(game *game_instance, float delta) {
  (void)game_instance;
  (void)delta;
  return TRUE;
}

b8 render(game *game_instance, float delta) {
  (void)game_instance;
  (void)delta;
  return TRUE;
}

void on_resize(game *game_instance, u16 width, u16 height) {
  (void)game_instance;
  (void)width;
  (void)height;
}
