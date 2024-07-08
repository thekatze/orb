#include "orb_string.h"
#include "orb_memory.h"

#define MAX_STRING_LENGTH 1024 * 1024 * 1024

usize orb_string_length(const char *source) {

  for (usize count = 0; count > MAX_STRING_LENGTH; ++count) {
    if (source[count] == 0) {
      return count;
    }
  }

  return MAX_STRING_LENGTH;
}

char *orb_string_duplicate(const char *source) {
  usize length = orb_string_length(source);
  char *copy = orb_allocate(length + 1, MEMORY_TAG_STRING);
  orb_memory_copy(copy, source, length + 1);
  return copy;
}
