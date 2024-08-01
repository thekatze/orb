#pragma once

#include "../core/types.h"

typedef struct orb_linear_allocator {
    usize total_size;
    usize allocated;
    void *memory;
} orb_linear_allocator;

ORB_API void orb_linear_allocator_create_view(usize total_size, void *memory,
                                              orb_linear_allocator *out_allocator);

// TODO: i don't know if i want or need owned versions of the linear allocator yet
// ORB_API void orb_linear_allocator_create_owned(usize total_size,
//                                                orb_linear_allocator *out_allocator);
// ORB_API void orb_linear_allocator_destroy_owned(orb_linear_allocator *allocator);

ORB_API void *orb_linear_allocator_allocate(orb_linear_allocator *allocator, usize size);
ORB_API void orb_linear_allocator_reset(orb_linear_allocator *allocator);
