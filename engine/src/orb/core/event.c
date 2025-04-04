#include "event.h"
#include "../containers/dynamic_array.h"
#include "asserts.h"
#include "orb_memory.h"

typedef struct registered_handler {
    void *listener;
    orb_event_handler_fn callback;
} registered_handler;

typedef struct event_code_handlers {
    orb_dynamic_array registered_event_handlers;
} event_code_entry;

#define MAX_MESSAGE_CODES 16384

typedef struct event_system_state {
    struct event_code_handlers registered[MAX_MESSAGE_CODES];
} event_system_state;

static event_system_state *state;

b8 orb_event_init(usize *memory_requirement, void *memory) {
    *memory_requirement = sizeof(event_system_state);
    if (memory == nullptr) {
        return true;
    }

    state = memory;
    orb_memory_zero(state, sizeof(event_system_state));

    return true;
}

void orb_event_shutdown() {
    for (u16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
        auto event_handlers = &state->registered[i].registered_event_handlers;

        // check if this dynamic_array has been initialized
        if (event_handlers->items != 0) {
            orb_dynamic_array_destroy(event_handlers);
        }
    }

    state = nullptr;
}

b8 orb_event_add_listener(event_code code, void *listener, orb_event_handler_fn on_event) {
    orb_dynamic_array *storage = &state->registered[code].registered_event_handlers;

    if (storage->items == 0) {
        *storage = orb_dynamic_array_create(registered_handler);
    }

// check for duplicate registration
#ifndef ORB_RELEASE
    registered_handler *handlers = (registered_handler *)storage->items;
    for (u64 i = 0; i < storage->length; ++i) {
        ORB_DEBUG_ASSERT(handlers[i].listener == listener, "listener already added for this event");
    }
#endif

    registered_handler handler = {0};
    handler.listener = listener;
    handler.callback = on_event;

    orb_dynamic_array_push(*storage, handler);

    return true;
}

b8 orb_event_remove_listener(event_code code, void *listener, orb_event_handler_fn on_event) {
    orb_dynamic_array *storage = &state->registered[code].registered_event_handlers;
    registered_handler *handlers = (registered_handler *)storage->items;

    if (handlers == 0) {
        // nothing has been registered for this code
        return false;
    }

    for (u64 i = 0; i < storage->length; ++i) {
        if (handlers[i].listener == listener && handlers[i].callback == on_event) {
            registered_handler removed;
            orb_dynamic_array_remove_at(storage, i, &removed);

            return true;
        }
    }

    return false;
}

b8 orb_event_send(event_code code, void *sender, orb_event_context context) {

    orb_dynamic_array *storage = &state->registered[code].registered_event_handlers;
    registered_handler *handlers = (registered_handler *)storage->items;

    if (handlers == nullptr) {
        // nothing has been registered for this code
        return false;
    }

    for (u64 i = 0; i < storage->length; ++i) {
        registered_handler *handler = &handlers[i];
        if (handler->callback(code, sender, handler->listener, context)) {
            return true;
        }
    }

    // no listener handled the event
    return false;
}
