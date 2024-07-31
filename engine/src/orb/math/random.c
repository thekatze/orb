#include "random.h"
#include "../core/asserts.h"

ORB_INLINE u64 rotate_left(u64 x, u8 shift) { return ((x) << (shift)) | ((x) >> (32 - (shift))); }

u64 jsf_next(orb_rng_state *x) {
    ORB_DEBUG_ASSERT(x->a | x->b | x->c | x->d,
                     "random number generator state was not initialized");

    u64 e = x->a - rotate_left(x->b, 27);
    x->a = x->b ^ rotate_left(x->c, 17);
    x->b = x->c + x->d;
    x->c = x->d + e;
    x->d = e + x->a;
    return x->d;
}

ORB_INLINE void jsf_init(orb_rng_state *x, u64 seed) {
    x->a = 0xf1ea5eed;
    x->b = seed;
    x->c = seed;
    x->d = seed;

    for (u8 i = 0; i < 20; ++i) {
        (void)jsf_next(x);
    }
}

void orb_random_init(struct orb_rng_state *rng, u64 seed) { jsf_init(rng, seed); }

i32 orb_random_int(struct orb_rng_state *rng) { return (i32)jsf_next(rng); }

i32 orb_random_int_within(struct orb_rng_state *rng, i32 min, i32 max) {
    return (orb_random_int(rng) % (max - min + 1)) + min;
}

const i32 INT32_MAX = 2'147'483'647;
f32 orb_random_float(struct orb_rng_state *rng) {
    i32 i = orb_random_int(rng);
    if (i < 0)
        i = -i;
    return (f32)i / (f32)INT32_MAX;
}

f32 orb_random_float_within(struct orb_rng_state *rng, f32 min, f32 max) {
    return (orb_random_float(rng) * (max - min)) + min;
}
