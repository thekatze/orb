#pragma once

#include "types.h"

ORB_API usize orb_string_length(const char *source);
ORB_API char *orb_string_duplicate(const char *source);

ORB_API b8 orb_string_equal(const char *this, const char *other);

ORB_API usize orb_string_format(char *destination, const char *format, ...);
ORB_API usize orb_string_format_v(char *destination, const char *format, void *va_list);
