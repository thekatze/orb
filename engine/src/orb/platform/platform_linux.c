#include "platform.h"
#include <bits/time.h>
#include <stdint.h>
#include <xcb/xproto.h>

#ifdef ORB_PLATFORM_LINUX

#include "../core/logger.h"

#include <X11/XKBlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <xcb/xcb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct internal_state {
  Display *display;
  xcb_connection_t *connection;
  xcb_window_t window;
  xcb_screen_t *screen;
  xcb_atom_t wm_protocols;
  xcb_atom_t wm_delete_win;
} internal_state;

b8 orb_platform_init(platform_state *platform, const char *application_name,
                     i32 x, i32 y, i32 width, i32 height) {
  platform->internal_state =
      orb_platform_allocate(sizeof(internal_state), false);
  internal_state *state = (internal_state *)platform->internal_state;

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

void orb_platform_shutdown(platform_state *platform) {
  internal_state *state = (internal_state *)platform->internal_state;

  XAutoRepeatOn(state->display);

  xcb_destroy_window(state->connection, state->window);

  orb_platform_free(state, FALSE);
}

b8 orb_platform_events_pump(platform_state *platform) {
  internal_state *state = (internal_state *)platform->internal_state;

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
      ORB_INFO("keyboard clicky");
    } break;
    case XCB_BUTTON_PRESS:
    case XCB_BUTTON_RELEASE: {
      ORB_INFO("mouse clicky");
    } break;
    case XCB_MOTION_NOTIFY: {
      ORB_INFO("mouse moved");
    } break;
    case XCB_CONFIGURE_WINDOW:
    case XCB_CONFIGURE_NOTIFY: {
      ORB_INFO("moved / resized");
    } break;
    case XCB_CLIENT_MESSAGE: {
      xcb_client_message_event_t *client_message =
          (xcb_client_message_event_t *)event;

      if (client_message->data.data32[0] == state->wm_delete_win) {
        should_quit = TRUE;
      }
    } break;
    default: {
      ORB_WARN("Unexpected xcb event: %i", event->response_type);
    }
    }

    free(event);
  }

  return !should_quit;
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

f64 orb_platform_time_elapsed() {
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

#endif
