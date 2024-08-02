#include "./orb_memory.h"
#include "./logger.h"

#include "../platform/platform.h"
#include "orb_string.h"

#include <stdio.h>

typedef struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
} memory_stats;

// TODO: in the kohi series this was moved to the subsystem linear allocator
// it would be great if this were in the linear allocator, but id rather track
// ALL allocations
// find a way to allow both (mounting/dismounting into the linear allocator?)
struct memory_system_state {
    memory_stats stats;
    usize allocation_count;
};

static struct memory_system_state state;

static const char *memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN           ", "ARRAY             ", "DYNAMIC_ARRAY     ", "DICTIONARY        ",
    "RING_QUEUE        ", "BINARY_SEARCH_TREE", "STRING            ", "APPLICATION       ",
    "JOB               ", "TEXTURE           ", "MATERIAL_INSTANCE ", "RENDERER          ",
    "GAME              ", "TRANSFORM         ", "ENTITY            ", "ENTITY_NODE       ",
    "SCENE             ",
};

void orb_memory_init() { orb_memory_zero(&state, sizeof(state)); }

void orb_memory_shutdown() {
#ifndef ORB_RELEASE
    // print leaked allocations
    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
        u64 allocation = state.stats.tagged_allocations[i];
        if (allocation != 0) {
            ORB_ERROR("%llu bytes of memory leaked in %s", allocation, memory_tag_strings[i]);
        }
    }
#endif
}

void *orb_allocate(u64 size, orb_memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        ORB_WARN("orb_allocate called using MEMORY_TAG_UNKNOWN.");
    }

    state.stats.total_allocated += size;
    state.stats.tagged_allocations[tag] += size;

    state.allocation_count += 1;

    void *block = orb_platform_allocate(size, false);

#ifndef ORB_RELEASE
    // in debug set memory to 010101...
    orb_memory_set(block, 0x5555'5555, size);
#endif

    return block;
}

void orb_free(void *block, u64 size, orb_memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        ORB_WARN("orb_free called using MEMORY_TAG_UNKNOWN.");
    }

    state.stats.total_allocated -= size;
    state.stats.tagged_allocations[tag] -= size;

    orb_platform_free(block, false);
}

void *orb_memory_zero(void *block, u64 size) { return orb_platform_memory_zero(block, size); }

void *orb_memory_copy(void *destination, const void *source, u64 size) {
    return orb_platform_memory_copy(destination, source, size);
}

void *orb_memory_set(void *destination, i32 value, u64 size) {
    return orb_platform_memory_set(destination, value, size);
}

char *orb_memory_debug_stats() {
    const u64 kib = 1024;
    const u64 mib = kib * kib;
    const u64 gib = kib * mib;

    char buffer[8000] = "System memory use (tagged): \n";

    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
        char unit[4] = "_iB";
        u64 allocation = state.stats.tagged_allocations[i];
        f32 displayed_allocation;

        if (allocation >= gib) {
            unit[0] = 'G';
            displayed_allocation = (f32)allocation / (f32)gib;
        } else if (allocation >= mib) {
            unit[0] = 'M';
            displayed_allocation = (f32)allocation / (f32)mib;
        } else if (allocation >= kib) {
            unit[0] = 'K';
            displayed_allocation = (f32)allocation / (f32)kib;
        } else {
            unit[0] = 'B';
            unit[1] = '\0';
            displayed_allocation = (f32)allocation;
        }

        u64 offset = orb_string_length(buffer);

        u64 written = (u64)snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i],
                                    displayed_allocation, unit);
        offset += written;
    }

    char *out_string = orb_string_duplicate(buffer);

    return out_string;
}

usize orb_memory_allocation_count() { return state.allocation_count; }
