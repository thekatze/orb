#pragma once

#ifdef ORB_ASSERTIONS_ENABLED

#include "types.h"

#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()

#else
#define debugBreak() __builtin_trap()

#endif

// defined in logger.c
ORB_API void report_assertion_failure(const char *expression, const char *message,
                              const char *file, i32 line);

#define ORB_ASSERT(expr, message)                                              \
  {                                                                            \
    if (!(expr)) {                                                             \
      report_assertion_failure(#expr, message, __FILE__, __LINE__);            \
      debugBreak();                                                            \
    }                                                                          \
  }

#if ORB_RELEASE == 0
#define ORB_DEBUG_ASSERT(expr, message)                                        \
  {                                                                            \
    if (!(expr)) {                                                             \
      report_assertion_failure(#expr, message, __FILE__, __LINE__);            \
      debugBreak();                                                            \
    }                                                                          \
  }
#else
#define ORB_DEBUG_ASSERT(expr, message)
#endif

#else

#define ORB_ASSERT(expr, message)
#define ORB_DEBUG_ASSERT(expr, message)

#endif
