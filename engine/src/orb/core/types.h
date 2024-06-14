#pragma once

#define DEFINE_NUMERIC_TYPE(bits, c_type)                                      \
  typedef unsigned c_type u##bits;                                             \
  typedef signed c_type i##bits;                                               \
  static_assert(sizeof(u##bits) == (bits) / 8);                                \
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
typedef u8 b8;

#define TRUE 1
#define FALSE 0

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

#ifdef _MSC_VER
#define ORB_API __declspec(dllexport)
#else
#define ORB_API __attribute__((visibility("default")))
#endif

#else

#ifdef _MSC_VER
#define ORB_API __declspec(dllimport)
#else
#define ORB_API
#endif

#endif
