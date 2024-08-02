#pragma once

#include "types.h"

// 128 bit, 16 bytes of data passed with an event
typedef struct orb_event_context {
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];
        char c[16];
    } data;
} orb_event_context;

b8 orb_event_init(usize *memory_requirement, void *memory);
void orb_event_shutdown();

typedef u16 event_code;

// returns true if event has been handled
typedef b8 (*orb_event_handler_fn)(event_code code, void *sender, void *listener,
                                   orb_event_context context);

ORB_API b8 orb_event_add_listener(event_code code, void *listener, orb_event_handler_fn on_event);

ORB_API b8 orb_event_remove_listener(event_code code, void *listener,
                                     orb_event_handler_fn on_event);

ORB_API b8 orb_event_send(event_code code, void *sender, orb_event_context context);

typedef enum orb_internal_event_code {
    /**
     * Usage:
     * u16 key_code = data.u16[0];
     * u16 repeat_count = data.u16[1];
     */
    ORB_EVENT_KEY_PRESSED = 0x01,

    /**
     * Usage:
     * u16 key_code = data.u16[0];
     * u16 repeat_count = data.u16[1];
     */
    ORB_EVENT_KEY_RELEASED = 0x02,

    /**
     * Usage:
     * u16 button = data.u16[0];
     * u16 x = data.i16[1];
     * u16 y = data.i16[2];
     */
    ORB_EVENT_MOUSE_BUTTON_PRESSED = 0x03,

    /**
     * Usage:
     * u16 button = data.u16[0];
     * u16 x = data.i16[1];
     * u16 y = data.i16[2];
     */
    ORB_EVENT_MOUSE_BUTTON_RELEASED = 0x04,

    /**
     * Usage:
     * u8 z = data.i8[0];
     */
    ORB_EVENT_MOUSE_WHEEL = 0x05,

    /**
     * Usage:
     * u16 x = data.i16[0];
     * u16 y = data.i16[1];
     */
    ORB_EVENT_MOUSE_MOVED = 0x08,

    /**
     * Usage:
     * u16 width = data.u16[0];
     * u16 height = data.u16[1];
     */
    ORB_EVENT_RESIZED = 0x10,
    ORB_EVENT_APPLICATION_QUIT = 0x1F,

    ORB_EVENT_MAX_CODE = 0xFF,
} orb_internal_event_code;
