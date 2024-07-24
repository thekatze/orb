#include "clock.h"
#include "../platform/platform.h"

void orb_clock_update(orb_clock *clock) {
    if ((i32)clock->start_time != 0) {
        clock->elapsed = orb_platform_time_now() - clock->start_time;
    }
}

void orb_clock_start(orb_clock *clock) {
    clock->start_time = orb_platform_time_now();
    clock->elapsed = 0;
}
void orb_clock_stop(orb_clock *clock) { clock->start_time = 0; }
