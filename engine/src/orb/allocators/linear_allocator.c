#include "linear_allocator.h"
#include "../core/asserts.h"
#include "../core/orb_memory.h"

void orb_linear_allocator_create_view(usize total_size, void *memory,
                                      orb_linear_allocator *out_allocator) {
    ORB_DEBUG_ASSERT(out_allocator != nullptr, "out_allocator must not be nullptr");
    ORB_DEBUG_ASSERT(memory != nullptr, "linear allocator view requires underlying memory");

    out_allocator->allocated = 0;
    out_allocator->total_size = total_size;
    out_allocator->memory = memory;
}

void *orb_linear_allocator_allocate(orb_linear_allocator *allocator, usize size) {
    ORB_DEBUG_ASSERT(allocator != nullptr, "allocator must not be nullptr");
    ORB_DEBUG_ASSERT(allocator->memory != nullptr, "allocator must be initialized");

    void *block = ((u8 *)allocator->memory) + allocator->allocated;
    allocator->allocated += size;

    ORB_ASSERT(allocator->allocated <= allocator->total_size, "out of memory");

    return block;
}

void orb_linear_allocator_reset(orb_linear_allocator *allocator) {
    ORB_DEBUG_ASSERT(allocator != nullptr, "allocator must not be nullptr");
    ORB_DEBUG_ASSERT(allocator->memory != nullptr, "allocator must be initialized");

#ifndef ORB_RELEASE
    orb_memory_set(allocator->memory, 0x5555'5555, allocator->allocated);
#endif
    allocator->allocated = 0;
}
