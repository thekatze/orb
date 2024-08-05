#include "orb_string.h"
#include "asserts.h"
#include "orb_memory.h"

#include <stdarg.h>
#include <stdio.h>

#define MAX_STRING_LENGTH 1024 * 1024 * 1024

usize orb_string_length(const char *source) {
    for (usize count = 0; count < MAX_STRING_LENGTH; ++count) {
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

b8 orb_string_equal(const char *this, const char *other) {
    // check if characters are equal until we reach both null terminators at the
    // same time.
    for (usize i = 0; this[i] != 0 || other[i] != 0; i++) {
        if (this[i] != other[i]) {
            return false;
        }
    }

    return true;
}

ORB_API usize orb_string_format(char *destination, const char *format, ...) {
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, format);

    usize written = orb_string_format_v(destination, format, arg_ptr);

    va_end(arg_ptr);
    return written;
}

ORB_API usize orb_string_format_v(char *destination, const char *format, void *va_list) {
    ORB_DEBUG_ASSERT(destination != nullptr, "destination must not be nullptr");

    const usize FORMAT_BUFFER_SIZE = 32000;
    char buffer[FORMAT_BUFFER_SIZE];
    i32 written = vsnprintf(buffer, FORMAT_BUFFER_SIZE, format, va_list);
    ORB_ASSERT(written != -1 && written, "buffer too small for resulting formatted string");
    buffer[written] = '\0';
    usize written_bytes = (usize)written;

    orb_memory_copy(destination, buffer, written_bytes + 1);

    return written_bytes;
}
