#pragma once

#define DEFINE_NUMERIC_TYPE(bits, c_type)                                                          \
    typedef unsigned c_type u##bits;                                                               \
    typedef signed c_type i##bits;                                                                 \
    static_assert(sizeof(u##bits) == (bits) / 8);                                                  \
    static_assert(sizeof(i##bits) == (bits) / 8);

DEFINE_NUMERIC_TYPE(8, char)
DEFINE_NUMERIC_TYPE(16, short)
DEFINE_NUMERIC_TYPE(32, int)
DEFINE_NUMERIC_TYPE(64, long long)

typedef float f32;
typedef double f64;
static_assert(sizeof(f32) == 32 / 8);
static_assert(sizeof(f64) == 64 / 8);

// TODO: make this actually system register width
typedef u64 usize;
typedef i64 isize;

// boolean
typedef bool b8;

#define ORB_ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

// detect platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define ORB_PLATFORM_WINDOWS

#elif defined(__linux__) || defined(__gnu_linux__)
#define ORB_PLATFORM_LINUX

#elif __APPLE__
#define ORB_PLATFORM_MAC

#else
#error "Unknown Platform"

#endif

// import/export dll functions with ORB_API

#ifdef ORB_EXPORT

#define ORB_API __attribute__((visibility("default")))
#else
#define ORB_API

#endif

// TODO: find a better way to do this. sadly statement expressions are a gnu
// extension https://stackoverflow.com/questions/3437404/min-and-max-in-c

#define ORB_MIN(left_expression, right_expression)                                                 \
    ((left_expression) < (right_expression) ? (left_expression) : (right_expression))

#define ORB_MAX(left_expression, right_expression)                                                 \
    ((left_expression) > (right_expression) ? (left_expression) : (right_expression))

#define ORB_CLAMP(value, min, max) ((value <= min) ? min : (value >= max) ? max : value)

// stolen from linux kernel: macro to influence code gen of if branches
#define likely(expression) __builtin_expect(!!(expression), 1)
#define unlikely(expression) __builtin_expect(!!(expression), 0)

#define ORB_INLINE static inline __attribute__((always_inline))
#define ORB_NOINLINE

