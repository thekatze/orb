#pragma once

#include "../core/types.h"

struct orb_application_config;

typedef struct orb_platform_state {
    void *internal_state;
} orb_platform_state;

[[nodiscard]]
b8 orb_platform_init(usize* memory_requirement, void* memory, struct orb_application_config* config);

void orb_platform_shutdown(orb_platform_state *platform);

[[nodiscard]]
b8 orb_platform_events_pump(orb_platform_state *platform);

[[nodiscard]]
void *orb_platform_allocate(u64 size, b8 aligned);
void orb_platform_free(void *block, b8 aligned);
void *orb_platform_memory_zero(void *block, u64 size);
void *orb_platform_memory_copy(void *destination, const void *source, u64 size);
void *orb_platform_memory_set(void *destination, i32 value, u64 size);

void orb_platform_console_write(const char *message, u8 color);
void orb_platform_console_write_error(const char *message, u8 color);

void orb_platform_get_window_handle_info(usize *out_size, void *memory);

[[nodiscard]]
f64 orb_platform_time_now();
void orb_platform_time_sleep(u64 ms);
