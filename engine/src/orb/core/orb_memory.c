#include "./orb_memory.h"
#include "./logger.h"

#include "../platform/platform.h"
#include "orb_string.h"

#include <stdio.h>

struct memory_stats {
  u64 total_allocated;
  u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static struct memory_stats stats;

void orb_memory_init() { orb_memory_zero(&stats, sizeof(stats)); }
void orb_memory_shutdown() {}

void *orb_allocate(u64 size, memory_tag tag) {
  if (tag == MEMORY_TAG_UNKNOWN) {
    ORB_WARN("orb_allocate called using MEMORY_TAG_UNKNOWN.");
  }

  stats.total_allocated += size;
  stats.tagged_allocations[tag] += size;

  void *block = orb_platform_allocate(size, FALSE);

#ifndef ORB_RELEASE
  // in debug set memory to 010101...
  orb_memory_set(block, 0x5555'5555, size);
#endif

  return block;
}

void orb_free(void *block, u64 size, memory_tag tag) {
  if (tag == MEMORY_TAG_UNKNOWN) {
    ORB_WARN("orb_free called using MEMORY_TAG_UNKNOWN.");
  }

  stats.total_allocated -= size;
  stats.tagged_allocations[tag] -= size;

  orb_platform_free(block, FALSE);
}

void *orb_memory_zero(void *block, u64 size) {
  return orb_platform_memory_zero(block, size);
}

void *orb_memory_copy(void *destination, const void *source, u64 size) {
  return orb_platform_memory_copy(destination, source, size);
}

void *orb_memory_set(void *destination, i32 value, u64 size) {
  return orb_platform_memory_set(destination, value, size);
}

static const char *memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN           ", "ARRAY             ", "DYNAMIC_ARRAY     ",
    "DICTIONARY        ", "RING_QUEUE        ", "BINARY_SEARCH_TREE",
    "STRING            ", "APPLICATION       ", "JOB               ",
    "TEXTURE           ", "MATERIAL_INSTANCE ", "RENDERER          ",
    "GAME              ", "TRANSFORM         ", "ENTITY            ",
    "ENTITY_NODE       ", "SCENE             ",
};

char *orb_memory_debug_stats() {
  const u64 kib = 1024;
  const u64 mib = kib * kib;
  const u64 gib = kib * mib;

  char buffer[8000] = "System memory use (tagged): \n";

  for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
    char unit[4] = "_iB";
    u64 allocation = stats.tagged_allocations[i];
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

    u64 written =
        (u64)snprintf(buffer + offset, 8000, "  %s: %.2f%s\n",
                      memory_tag_strings[i], displayed_allocation, unit);
    offset += written;
  }

  char *out_string = orb_string_duplicate(buffer);

  return out_string;
}
