#pragma once

#include "../core/types.h"

typedef struct orb_linear_allocator {
    usize total_size;
    usize allocated;
    void *memory;
} orb_linear_allocator;

ORB_API void orb_linear_allocator_create(usize total_size, void *memory,
                                         orb_linear_allocator *out_allocator);

ORB_API void *orb_linear_allocator_allocate(orb_linear_allocator *allocator, usize size);
ORB_API void orb_linear_allocator_reset(orb_linear_allocator *allocator);
