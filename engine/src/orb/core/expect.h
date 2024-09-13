#pragma once

#include "logger.h"

#ifdef ORB_RELEASE
#define ORB_EXPECT(fallible_call, error_message)                                                   \
    do {                                                                                           \
        b8 result = fallible_call;                                                                 \
        if (unlikely(!result)) {                                                                   \
            ORB_ERROR(error_message);                                                              \
            return false;                                                                          \
        }                                                                                          \
    } while (0)
#else
#define ORB_EXPECT(fallible_call, error_message)                                                   \
    do {                                                                                           \
        b8 result = fallible_call;                                                                 \
        if (unlikely(!result)) {                                                                   \
            ORB_ERROR("%s:%d " error_message, __FILE__, __LINE__);                                 \
            return false;                                                                          \
        }                                                                                          \
    } while (0)
#endif
