#pragma once

#include <orb/core/logger.h>
#include <orb/core/types.h>
#include <orb/math/orb_math.h>

#define expect_eq(expected, actual)                                                                \
    do {                                                                                           \
        if ((actual) != (expected)) {                                                              \
            ORB_ERROR("  %s:%d expect_eq failed: expected " #expected " (= %lld) but got %lld",    \
                      __FILE__, __LINE__, expected, actual);                                       \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define expect_neq(expected, actual)                                                               \
    do {                                                                                           \
        if ((actual) == (expected)) {                                                              \
            ORB_ERROR("  %s:%d expect_neq failed: expected %lld to differ from " #expected         \
                      "(%lld)",                                                                    \
                      __FILE__, __LINE__, actual, expected);                                       \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define expect_float_eq(expected, actual, tolerance)                                               \
    do {                                                                                           \
        if (orb_abs(expected - actual) > tolerance) {                                              \
            ORB_ERROR("  %s:%d expect_float_eq failed: expected " #expected " (= %f) but got %f",  \
                      __FILE__, __LINE__, expected, actual);                                       \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define expect(expected)                                                                           \
    do {                                                                                           \
        if (!(expected)) {                                                                         \
            ORB_ERROR("  %s:%d expect failed: expected " #expected " to be true", __FILE__,        \
                      __LINE__);                                                                   \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define expect_false(expected)                                                                     \
    do {                                                                                           \
        if (expected) {                                                                            \
            ORB_ERROR("  %s:%d expect_false failed: expected " #expected " to be false", __FILE__, \
                      __LINE__);                                                                   \
            return false;                                                                          \
        }                                                                                          \
    } while (0)
