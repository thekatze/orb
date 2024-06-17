#pragma once

#include "../core/types.h"

typedef struct platform_state {
  void *internal_state;
} platform_state;

b8 orb_platform_init(platform_state *platform, const char *application_name,
                     i32 x, i32 y, i32 width, i32 height);

void orb_platform_shutdown(platform_state *platform);

b8 orb_platform_events_pump(platform_state *platform);

ORB_API void *orb_platform_allocate(u64 size, b8 aligned);
ORB_API void orb_platform_free(void *block, b8 aligned);
void *orb_platform_memory_zero(void *block, u64 size);
void *orb_platform_memory_copy(void *destination, const void *source, u64 size);
void *orb_platform_memory_set(void *destination, i32 value, u64 size);

void orb_platform_console_write(const char *message, u8 color);
void orb_platform_console_write_error(const char *message, u8 color);

f64 orb_platform_time_elapsed();
void orb_platform_time_sleep(u64 ms);
