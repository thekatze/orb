#pragma once

#include "./types.h"

typedef enum orb_memory_tag {
    MEMORY_TAG_UNKNOWN = 0,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DYNAMIC_ARRAY,
    MEMORY_TAG_DICTIONARY,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BINARY_SEARCH_TREE,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS,
} orb_memory_tag;

void orb_memory_init();
void orb_memory_shutdown();

ORB_API void *orb_allocate(u64 size, orb_memory_tag tag);
ORB_API void orb_free(void *block, u64 size, orb_memory_tag tag);
ORB_API void *orb_memory_zero(void *block, u64 size);
ORB_API void *orb_memory_copy(void *destination, const void *source, u64 size);
ORB_API void *orb_memory_set(void *destination, i32 value, u64 size);

// must be freed by caller
ORB_API char *orb_memory_debug_stats();
