#include "input.h"
#include "event.h"
#include "orb_memory.h"

typedef struct keyboard_state {
  b8 keys[KEYS_MAX_KEYS];
} keyboard_state;

typedef struct mouse_state {
  i16 x;
  i16 y;
  b8 buttons[MOUSE_BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state {
  keyboard_state current_keyboard;
  mouse_state current_mouse;
  keyboard_state previous_keyboard;
  mouse_state previous_mouse;
} input_state;

static input_state state = {0};

b8 orb_input_init() { return TRUE; }
void orb_input_shutdown() {}

void orb_input_update() {
  orb_memory_copy(&state.previous_keyboard, &state.current_keyboard,
                  sizeof(keyboard_state));
  orb_memory_copy(&state.previous_mouse, &state.current_mouse,
                  sizeof(mouse_state));
}

// is the key down
b8 orb_input_is_key_down(orb_keyboard_keys key) {
  return state.current_keyboard.keys[key];
}

// did the key get pressed this frame
b8 orb_input_is_key_pressed(orb_keyboard_keys key) {
  return state.current_keyboard.keys[key] && !state.previous_keyboard.keys[key];
}

// is the key up
b8 orb_input_is_key_up(orb_keyboard_keys key) {
  return !state.current_keyboard.keys[key];
}

// did the key get released this frame
b8 orb_input_is_key_released(orb_keyboard_keys key) {
  return !state.current_keyboard.keys[key] && state.previous_keyboard.keys[key];
}

void orb_input_process_key(orb_keyboard_keys key, b8 pressed) {
  if (state.current_keyboard.keys[key] == pressed)
    return; // nothing changed, no need to fire an event

  state.current_keyboard.keys[key] = pressed;

  event_context context;
  context.data.u16[0] = key;
  context.data.u16[1] = 0; // repeat count

  orb_event_send(pressed ? ORB_EVENT_KEY_PRESSED : ORB_EVENT_KEY_RELEASED, 0,
                 context);
}

// is the button down
b8 orb_input_is_mouse_button_down(orb_mouse_buttons button) {
  return state.current_mouse.buttons[button];
}

// did the button get pressed this frame
b8 orb_input_is_mouse_button_pressed(orb_mouse_buttons button) {
  return state.current_mouse.buttons[button] &&
         !state.previous_mouse.buttons[button];
}

// is the button up
b8 orb_input_is_mouse_button_up(orb_mouse_buttons button) {
  return !state.current_mouse.buttons[button];
}

// did the button get released this frame
b8 orb_input_is_mouse_button_released(orb_mouse_buttons button) {
  return !state.current_mouse.buttons[button] &&
         state.previous_mouse.buttons[button];
}

void orb_input_get_mouse_position(i32 *x, i32 *y) {
  *x = state.current_mouse.x;
  *y = state.current_mouse.y;
}

void orb_input_get_mouse_delta(i32 *x, i32 *y) {
  *x = state.previous_mouse.x - state.current_mouse.x;
  *y = state.previous_mouse.y - state.current_mouse.y;
}

void orb_input_process_mouse_button(orb_mouse_buttons button, b8 pressed) {
  if (state.current_mouse.buttons[button] == pressed)
    return; // nothing changed, no need to fire an event

  state.current_mouse.buttons[button] = pressed;

  event_context context;
  context.data.u16[0] = button;
  context.data.i16[1] = state.current_mouse.x;
  context.data.i16[2] = state.current_mouse.y;

  orb_event_send(pressed ? ORB_EVENT_MOUSE_BUTTON_PRESSED
                         : ORB_EVENT_MOUSE_BUTTON_RELEASED,
                 0, context);
}

void orb_input_process_mouse_move(i16 x, i16 y) {
  if (state.current_mouse.x == x && state.current_mouse.y == y)
    return; // nothing changed, no need to fire an event

  state.current_mouse.x = x;
  state.current_mouse.y = y;

  event_context context;
  context.data.i16[0] = x;
  context.data.i16[1] = y;

  orb_event_send(ORB_EVENT_MOUSE_MOVED, 0, context);
}

void orb_input_process_mouse_wheel(i8 z_delta) {
  event_context context;
  context.data.i8[0] = z_delta;
  orb_event_send(ORB_EVENT_MOUSE_WHEEL, 0, context);
}
