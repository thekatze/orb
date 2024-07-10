#include "platform.h"

#ifdef ORB_PLATFORM_LINUX

#include "../core/input.h"
#include "../core/logger.h"

// TODO: Validate if these are needed
#include <bits/time.h>
#include <stdint.h>
#include <xcb/xproto.h>

#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <xcb/xcb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct orb_internal_state {
  Display *display;
  xcb_connection_t *connection;
  xcb_window_t window;
  xcb_screen_t *screen;
  xcb_atom_t wm_protocols;
  xcb_atom_t wm_delete_win;
} orb_internal_state;

orb_internal_state *state = 0;

orb_keyboard_keys translate_keycode(KeySym key_sym);

b8 orb_platform_init(orb_platform_state *platform, const char *application_name,
                     i32 x, i32 y, i32 width, i32 height) {
  platform->internal_state =
      orb_platform_allocate(sizeof(orb_internal_state), false);
  state = (orb_internal_state *)platform->internal_state;

  state->display = XOpenDisplay(NULL);

  // turn off key repeats.
  XAutoRepeatOff(state->display);

  state->connection = XGetXCBConnection(state->display);

  if (xcb_connection_has_error(state->connection)) {
    ORB_FATAL("Failed to connect to X server.");
    return FALSE;
  }

  const struct xcb_setup_t *setup = xcb_get_setup(state->connection);

  xcb_screen_iterator_t screens = xcb_setup_roots_iterator(setup);

  // for (i32 s = 0; s > 0; s--) {
  //   xcb_screen_next(&screens);
  // }

  state->screen = screens.data;

  state->window = xcb_generate_id(state->connection);

  // register event types
  u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS |
                     XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS |
                     XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE |
                     XCB_EVENT_MASK_POINTER_MOTION |
                     XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  // send background colors and requested events to xcb
  u32 value_list[] = {state->screen->black_pixel, event_values};
  // xcb_void_cookie_t cookie =
  xcb_create_window(state->connection, XCB_COPY_FROM_PARENT, state->window,
                    state->screen->root, (i16)x, (i16)y, (u16)width,
                    (u16)height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    state->screen->root_visual, event_mask, value_list);

  xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, sizeof(char) * 8,
                      (u32)strlen(application_name), application_name);

  const char *WM_DELETE_WINDOW = "WM_DELETE_WINDOW";
  xcb_intern_atom_cookie_t wm_delete_cookie =
      xcb_intern_atom(state->connection, FALSE, (u16)strlen(WM_DELETE_WINDOW),
                      WM_DELETE_WINDOW);

  const char *WM_PROTOCOLS = "WM_PROTOCOLS";
  xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
      state->connection, FALSE, (u16)strlen(WM_PROTOCOLS), WM_PROTOCOLS);

  xcb_intern_atom_reply_t *wm_delete_reply =
      xcb_intern_atom_reply(state->connection, wm_delete_cookie, NULL);

  xcb_intern_atom_reply_t *wm_protocols_reply =
      xcb_intern_atom_reply(state->connection, wm_protocols_cookie, NULL);

  state->wm_delete_win = wm_delete_reply->atom;
  state->wm_protocols = wm_protocols_reply->atom;

  xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window,
                      wm_protocols_reply->atom, XCB_ATOM_ATOM,
                      sizeof(xcb_atom_t) * 8, 1, &wm_delete_reply->atom);

  xcb_map_window(state->connection, state->window);

  i32 stream_result = xcb_flush(state->connection);
  if (stream_result <= 0) {
    ORB_FATAL("An error occurred when flushing the xcb stream: %d",
              stream_result);
    return FALSE;
  }

  return TRUE;
}

void orb_platform_shutdown(orb_platform_state *platform) {
  (void)platform;
  XAutoRepeatOn(state->display);

  xcb_destroy_window(state->connection, state->window);

  orb_platform_free(state, FALSE);
}

b8 orb_platform_events_pump(orb_platform_state *platform) {
  (void)platform;

  b8 should_quit = FALSE;

  while (TRUE) {
    xcb_generic_event_t *event = xcb_poll_for_event(state->connection);
    if (!event) {
      // no more events to handle
      break;
    }

    switch (event->response_type & ~0x80) {
    case XCB_KEY_PRESS:
    case XCB_KEY_RELEASE: {
      xcb_key_press_event_t *keyboard_event = (xcb_key_press_event_t *)event;
      b8 pressed = event->response_type == XCB_KEY_PRESS;
      xcb_keycode_t code = keyboard_event->detail;
      KeySym symbol = XkbKeycodeToKeysym(state->display, (KeyCode)code, 0,
                                         code & ShiftMask ? 1 : 0);

      orb_keyboard_keys key = translate_keycode(symbol);
      orb_input_process_key(key, pressed);
    } break;
    case XCB_BUTTON_PRESS:
    case XCB_BUTTON_RELEASE: {
      xcb_button_press_event_t *mouse_event = (xcb_button_press_event_t *)event;
      b8 pressed = event->response_type == XCB_BUTTON_PRESS;
      orb_mouse_buttons mouse_button = MOUSE_BUTTON_MAX_BUTTONS;

      switch (mouse_event->detail) {
      case XCB_BUTTON_INDEX_1:
        mouse_button = MOUSE_BUTTON_LEFT;
        break;
      case XCB_BUTTON_INDEX_2:
        mouse_button = MOUSE_BUTTON_MIDDLE;
        break;
      case XCB_BUTTON_INDEX_3:
        mouse_button = MOUSE_BUTTON_RIGHT;
        break;
      }

      if (mouse_button == MOUSE_BUTTON_MAX_BUTTONS) {
        ORB_WARN("Unknown mouse button index: %i", mouse_event->detail);
        continue;
      }

      orb_input_process_mouse_button(mouse_button, pressed);
    } break;
    case XCB_MOTION_NOTIFY: {
      xcb_motion_notify_event_t *mouse_move_event =
          (xcb_motion_notify_event_t *)event;

      orb_input_process_mouse_move(mouse_move_event->event_x,
                                   mouse_move_event->event_y);
    } break;
    case XCB_CONFIGURE_WINDOW:
    case XCB_CONFIGURE_NOTIFY: {
      // ORB_INFO("moved / resized");
    } break;
    case XCB_CLIENT_MESSAGE: {
      xcb_client_message_event_t *client_message =
          (xcb_client_message_event_t *)event;

      if (client_message->data.data32[0] == state->wm_delete_win) {
        should_quit = TRUE;
      }
    } break;
    default: {
      // ORB_WARN("Unexpected xcb event: %i", event->response_type);
    }
    }

    free(event);
  }

  return !should_quit;
}

typedef struct linux_handle_info {
  xcb_connection_t *connection;
  xcb_window_t window;
} linux_handle_info;

void orb_platform_get_window_handle_info(usize *out_size, void *memory) {
  *out_size = sizeof(linux_handle_info);
  if (memory == nullptr) {
    return;
  }

  linux_handle_info *out_typed = (linux_handle_info *)memory;
  out_typed->connection = state->connection;
  out_typed->window = state->window;
}

void *orb_platform_allocate(u64 size, b8 aligned) {
  (void)aligned;
  return malloc(size);
}

void orb_platform_free(void *block, b8 aligned) {
  (void)aligned;
  free(block);
}

void *orb_platform_memory_zero(void *block, u64 size) {
  return orb_platform_memory_set(block, 0, size);
}

void *orb_platform_memory_copy(void *destination, const void *source,
                               u64 size) {
  return memcpy(destination, source, size);
}

void *orb_platform_memory_set(void *destination, i32 value, u64 size) {
  return memset(destination, value, size);
}

// FATAL, ERROR, WARN, INFO, DEBUG, TRACE
const char *color_escape_codes[] = {"0;41", "1;31", "1;33",
                                    "1;32", "1;34", "1;30"};

void orb_platform_console_write(const char *message, u8 color) {
  printf("\033[%sm%s\033[0m", color_escape_codes[color], message);
}

void orb_platform_console_write_error(const char *message, u8 color) {
  fprintf(stderr, "\033[%sm%s\033[0m", color_escape_codes[color], message);
}

f64 orb_platform_time_now() {
  const f64 NANOSECONDS_IN_SECONDS = 1.0 / 1'000'000'000;
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (f64)now.tv_sec + (f64)now.tv_nsec * NANOSECONDS_IN_SECONDS;
}

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
void orb_platform_time_sleep(u64 ms) {
  struct timespec ts;
  ts.tv_sec = (i64)ms / 1000;
  ts.tv_nsec = ((i64)ms % 1000) * 1000 * 1000;
  nanosleep(&ts, 0);
}
#else
#include <unistd.h>
void orb_platform_time_sleep(u64 ms) {
  if (ms >= 1000) {
    sleep(ms / 1000);
  }
  usleep((ms % 1000) * 1000);
}
#endif

orb_keyboard_keys translate_keycode(KeySym x_keycode) {
  switch (x_keycode) {
  case XK_BackSpace:
    return KEY_BACKSPACE;
  case XK_Return:
    return KEY_ENTER;
  case XK_Tab:
    return KEY_TAB;

  case XK_Pause:
    return KEY_PAUSE;
  case XK_Caps_Lock:
    return KEY_CAPITAL;

  case XK_Escape:
    return KEY_ESCAPE;

  case XK_Mode_switch:
    return KEY_MODECHANGE;

  case XK_space:
    return KEY_SPACE;
  case XK_Prior:
    return KEY_PAGEUP;
  case XK_Next:
    return KEY_PAGEDOWN;
  case XK_End:
    return KEY_END;
  case XK_Home:
    return KEY_HOME;
  case XK_Left:
    return KEY_LEFT;
  case XK_Up:
    return KEY_UP;
  case XK_Right:
    return KEY_RIGHT;
  case XK_Down:
    return KEY_DOWN;
  case XK_Select:
    return KEY_SELECT;
  case XK_Print:
    return KEY_PRINT;
  case XK_Execute:
    return KEY_EXECUTE;
  case XK_Insert:
    return KEY_INSERT;
  case XK_Delete:
    return KEY_DELETE;
  case XK_Help:
    return KEY_HELP;

  case XK_Meta_L:
    return KEY_LSUPER;
  case XK_Meta_R:
    return KEY_RSUPER;

  case XK_KP_0:
    return KEY_NUMPAD0;
  case XK_KP_1:
    return KEY_NUMPAD1;
  case XK_KP_2:
    return KEY_NUMPAD2;
  case XK_KP_3:
    return KEY_NUMPAD3;
  case XK_KP_4:
    return KEY_NUMPAD4;
  case XK_KP_5:
    return KEY_NUMPAD5;
  case XK_KP_6:
    return KEY_NUMPAD6;
  case XK_KP_7:
    return KEY_NUMPAD7;
  case XK_KP_8:
    return KEY_NUMPAD8;
  case XK_KP_9:
    return KEY_NUMPAD9;
  case XK_multiply:
    return KEY_MULTIPLY;
  case XK_KP_Add:
    return KEY_ADD;
  case XK_KP_Separator:
    return KEY_SEPARATOR;
  case XK_KP_Subtract:
    return KEY_SUBTRACT;
  case XK_KP_Decimal:
    return KEY_DECIMAL;
  case XK_KP_Divide:
    return KEY_DIVIDE;
  case XK_F1:
    return KEY_F1;
  case XK_F2:
    return KEY_F2;
  case XK_F3:
    return KEY_F3;
  case XK_F4:
    return KEY_F4;
  case XK_F5:
    return KEY_F5;
  case XK_F6:
    return KEY_F6;
  case XK_F7:
    return KEY_F7;
  case XK_F8:
    return KEY_F8;
  case XK_F9:
    return KEY_F9;
  case XK_F10:
    return KEY_F10;
  case XK_F11:
    return KEY_F11;
  case XK_F12:
    return KEY_F12;
  case XK_F13:
    return KEY_F13;
  case XK_F14:
    return KEY_F14;
  case XK_F15:
    return KEY_F15;
  case XK_F16:
    return KEY_F16;
  case XK_F17:
    return KEY_F17;
  case XK_F18:
    return KEY_F18;
  case XK_F19:
    return KEY_F19;
  case XK_F20:
    return KEY_F20;
  case XK_F21:
    return KEY_F21;
  case XK_F22:
    return KEY_F22;
  case XK_F23:
    return KEY_F23;
  case XK_F24:
    return KEY_F24;

  case XK_Num_Lock:
    return KEY_NUMLOCK;
  case XK_Scroll_Lock:
    return KEY_SCROLL;

  case XK_KP_Equal:
    return KEY_NUMPAD_EQUAL;

  case XK_Shift_L:
    return KEY_LSHIFT;
  case XK_Shift_R:
    return KEY_RSHIFT;
  case XK_Control_L:
    return KEY_LCONTROL;
  case XK_Control_R:
    return KEY_RCONTROL;
  case XK_Alt_L:
    return KEY_LALT;
  case XK_Alt_R:
    return KEY_RALT;

  case XK_semicolon:
    return KEY_SEMICOLON;
  case XK_plus:
    return KEY_EQUAL;
  case XK_comma:
    return KEY_COMMA;
  case XK_minus:
    return KEY_MINUS;
  case XK_period:
    return KEY_PERIOD;
  case XK_slash:
    return KEY_SLASH;
  case XK_grave:
    return KEY_GRAVE;

  case XK_0:
    return KEY_0;
  case XK_1:
    return KEY_1;
  case XK_2:
    return KEY_2;
  case XK_3:
    return KEY_3;
  case XK_4:
    return KEY_4;
  case XK_5:
    return KEY_5;
  case XK_6:
    return KEY_6;
  case XK_7:
    return KEY_7;
  case XK_8:
    return KEY_8;
  case XK_9:
    return KEY_9;

  case XK_a:
  case XK_A:
    return KEY_A;
  case XK_b:
  case XK_B:
    return KEY_B;
  case XK_c:
  case XK_C:
    return KEY_C;
  case XK_d:
  case XK_D:
    return KEY_D;
  case XK_e:
  case XK_E:
    return KEY_E;
  case XK_f:
  case XK_F:
    return KEY_F;
  case XK_g:
  case XK_G:
    return KEY_G;
  case XK_h:
  case XK_H:
    return KEY_H;
  case XK_i:
  case XK_I:
    return KEY_I;
  case XK_j:
  case XK_J:
    return KEY_J;
  case XK_k:
  case XK_K:
    return KEY_K;
  case XK_l:
  case XK_L:
    return KEY_L;
  case XK_m:
  case XK_M:
    return KEY_M;
  case XK_n:
  case XK_N:
    return KEY_N;
  case XK_o:
  case XK_O:
    return KEY_O;
  case XK_p:
  case XK_P:
    return KEY_P;
  case XK_q:
  case XK_Q:
    return KEY_Q;
  case XK_r:
  case XK_R:
    return KEY_R;
  case XK_s:
  case XK_S:
    return KEY_S;
  case XK_t:
  case XK_T:
    return KEY_T;
  case XK_u:
  case XK_U:
    return KEY_U;
  case XK_v:
  case XK_V:
    return KEY_V;
  case XK_w:
  case XK_W:
    return KEY_W;
  case XK_x:
  case XK_X:
    return KEY_X;
  case XK_y:
  case XK_Y:
    return KEY_Y;
  case XK_z:
  case XK_Z:
    return KEY_Z;
  }

  return KEYS_MAX_KEYS;
}

#endif
