#pragma once

#include "types.h"

[[nodiscard]]
ORB_API usize orb_string_length(const char *source);

[[nodiscard]]
ORB_API char *orb_string_duplicate(const char *source);

[[nodiscard]]
ORB_API b8 orb_string_equal(const char *this, const char *other);

ORB_API usize orb_string_format(char *destination, const char *format, ...);
ORB_API usize orb_string_format_v(char *destination, const char *format, void *va_list);
