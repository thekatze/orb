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

