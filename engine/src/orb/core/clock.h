#pragma once
#include "types.h"

typedef struct orb_clock {
    f64 start_time;
    // elapsed time in seconds
    f64 elapsed;
} orb_clock;

ORB_API void orb_clock_update(orb_clock *clock);
ORB_API void orb_clock_start(orb_clock *clock);
ORB_API void orb_clock_stop(orb_clock *clock);
