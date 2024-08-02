#pragma once

#ifdef ORB_ASSERTIONS_ENABLED

#include "types.h"

#define orb_debug_break() __builtin_trap()

// defined in logger.c
ORB_API void orb_report_assertion_failure(const char *expression, const char *message,
                                          const char *file, i32 line);

#define ORB_ASSERT(expr, message)                                                                  \
    {                                                                                              \
        if (unlikely(!(expr))) {                                                                   \
            orb_report_assertion_failure(#expr, message, __FILE__, __LINE__);                      \
            orb_debug_break();                                                                     \
        }                                                                                          \
    }

#ifndef ORB_RELEASE
#define ORB_DEBUG_ASSERT(expr, message)                                                            \
    {                                                                                              \
        if (unlikely(!(expr))) {                                                                   \
            orb_report_assertion_failure(#expr, message, __FILE__, __LINE__);                      \
            orb_debug_break();                                                                     \
        }                                                                                          \
    }
#else
#define ORB_DEBUG_ASSERT(expr, message)
#endif

#else

#define ORB_ASSERT(expr, message)
#define ORB_DEBUG_ASSERT(expr, message)

#endif
