#pragma once

#include "../core/types.h"

// the random number generator is JSF
// http://burtleburtle.net/bob/rand/smallprng.html
typedef struct orb_rng_state {
    u64 a;
    u64 b;
    u64 c;
    u64 d;
} orb_rng_state;

ORB_API void orb_random_init(orb_rng_state *rng, u64 seed);

ORB_API i32 orb_random_int(orb_rng_state *rng);
ORB_API i32 orb_random_int_within(orb_rng_state *rng, i32 min, i32 max);

// @brief returns a random number between 0 and 1
ORB_API f32 orb_random_float(orb_rng_state *rng);
ORB_API f32 orb_random_float_within(orb_rng_state *rng, f32 min, f32 max);
