#pragma once

#include "../core/asserts.h"
#include "math_types.h"

#define ORB_PI 3.14159265358979323846f
#define ORB_TAU (2.0f * ORB_PI)
#define ORB_HALF_PI (0.5f * ORB_PI)
#define ORB_QUARTER_PI (0.25f * ORB_PI)
#define ORB_PI_RECIP (1.0f / ORB_PI)
#define ORB_TAU_RECIP (1.0f / ORB_TAU)

#define ORB_SQRT_TWO 1.41421356237309504880f
#define ORB_SQRT_THREE 1.73205080756887729352f
#define ORB_SQRT_HALF 0.707106781186547524401f
#define ORB_SQRT_THIRD 0.577350269189625764509f

#define ORB_FLOAT_EPSILON 1.192092896e-07f

ORB_API f32 orb_sin(f32 x);
ORB_API f32 orb_cos(f32 x);
ORB_API f32 orb_tan(f32 x);
ORB_API f32 orb_acos(f32 x);
ORB_API f32 orb_sqrt(f32 x);
ORB_API f32 orb_abs(f32 x);

ORB_INLINE b8 orb_is_power_of_2(u64 value) { return (value != 0) && ((value & (value - 1)) == 0); }

ORB_INLINE f32 orb_degrees_to_radians(f32 degrees) { return degrees * ORB_PI / 180.0f; }
ORB_INLINE f32 orb_radians_to_degrees(f32 radians) { return radians * 180.0f / ORB_PI; }

// TODO:
// Split into SIMD and non-SIMD

#ifdef ORB_SIMD
#if ORB_PLATFORM_MAC
// #include <orb_math_neon.h>
#else
// #include <orb_math_avx.h>
#endif

#else
// #include <orb_math_fallback.h>
#endif

ORB_INLINE orb_vec2 orb_vec2_new(f32 x, f32 y) {
    return (orb_vec2){
        .x = x,
        .y = y,
    };
}

ORB_INLINE orb_vec2 orb_vec2_zero() {
    return (orb_vec2){
        .x = 0,
        .y = 0,
    };
}

ORB_INLINE orb_vec2 orb_vec2_right() {
    return (orb_vec2){
        .x = 1,
        .y = 0,
    };
}

ORB_INLINE orb_vec2 orb_vec2_left() {
    return (orb_vec2){
        .x = -1,
        .y = 0,
    };
}

ORB_INLINE orb_vec2 orb_vec2_up() {
    return (orb_vec2){
        .x = 0,
        .y = 1,
    };
}

ORB_INLINE orb_vec2 orb_vec2_down() {
    return (orb_vec2){
        .x = 0,
        .y = -1,
    };
}

ORB_INLINE orb_vec2 orb_vec2_add(orb_vec2 lhs, orb_vec2 rhs) {
    return (orb_vec2){
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y,
    };
}

ORB_INLINE orb_vec2 orb_vec2_sub(orb_vec2 lhs, orb_vec2 rhs) {
    return (orb_vec2){
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
    };
}

ORB_INLINE orb_vec2 orb_vec2_scale(orb_vec2 lhs, f32 rhs) {
    return (orb_vec2){
        .x = lhs.x * rhs,
        .y = lhs.y * rhs,
    };
}

/**
 * @brief dot product.
 */
ORB_INLINE f32 orb_vec2_dot(orb_vec2 lhs, orb_vec2 rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }

/**
 * @brief element-wise multiplication.
 */
ORB_INLINE orb_vec2 orb_vec2_hadamard_prod(orb_vec2 lhs, orb_vec2 rhs) {
    return (orb_vec2){
        .x = lhs.x * rhs.x,
        .y = lhs.y * rhs.y,
    };
}

/**
 * @brief element-wise division.
 */
ORB_INLINE orb_vec2 orb_vec2_hadamard_div(orb_vec2 lhs, orb_vec2 rhs) {
    return (orb_vec2){
        .x = lhs.x / rhs.x,
        .y = lhs.y / rhs.y,
    };
}

ORB_INLINE f32 orb_vec2_magnitude_sqr(orb_vec2 vec) { return vec.x * vec.x + vec.y * vec.y; }
ORB_INLINE f32 orb_vec2_magnitude(orb_vec2 vec) { return orb_sqrt(orb_vec2_magnitude_sqr(vec)); }

/**
 * @brief normalize in place
 */
ORB_INLINE void orb_vec2_normalize(orb_vec2 *vec) {
    const f32 length = orb_vec2_magnitude(*vec);
    vec->x /= length;
    vec->y /= length;
}

/**
 * @brief create new normalized vector
 */
ORB_INLINE orb_vec2 orb_vec2_normalized(orb_vec2 vec) {
    orb_vec2_normalize(&vec);
    return vec;
}

/**
 * @brief returns true if all elements are equal within some tolerance; usually ORB_FLOAT_EPSILON
 */
ORB_INLINE b8 orb_vec2_equal(const orb_vec2 *lhs, const orb_vec2 *rhs, f32 tolerance) {
    if (orb_abs(lhs->x - rhs->x) > tolerance) {
        return false;
    }
    if (orb_abs(lhs->y - rhs->y) > tolerance) {
        return false;
    }

    return true;
}

ORB_INLINE f32 orb_vec2_distance(orb_vec2 lhs, orb_vec2 rhs) {
    return orb_vec2_magnitude(orb_vec2_sub(lhs, rhs));
}

//
// vec3
//

ORB_INLINE orb_vec3 orb_vec3_new(f32 x, f32 y, f32 z) {
    return (orb_vec3){
        .x = x,
        .y = y,
        .z = z,
    };
}

ORB_INLINE orb_vec3 orb_vec3_zero() {
    return (orb_vec3){
        .x = 0,
        .y = 0,
        .z = 0,
    };
}

ORB_INLINE orb_vec3 orb_vec3_x() {
    return (orb_vec3){
        .x = 1,
        .y = 0,
        .z = 0,
    };
}

ORB_INLINE orb_vec3 orb_vec3_neg_x() {
    return (orb_vec3){
        .x = -1,
        .y = 0,
        .z = 0,
    };
}

ORB_INLINE orb_vec3 orb_vec3_y() {
    return (orb_vec3){
        .x = 0,
        .y = 1,
        .z = 0,
    };
}

ORB_INLINE orb_vec3 orb_vec3_neg_y() {
    return (orb_vec3){
        .x = 0,
        .y = -1,
        .z = 0,
    };
}

ORB_INLINE orb_vec3 orb_vec3_z() {
    return (orb_vec3){
        .x = 0,
        .y = 0,
        .z = 1,
    };
}

ORB_INLINE orb_vec3 orb_vec3_neg_z() {
    return (orb_vec3){
        .x = 0,
        .y = 0,
        .z = -1,
    };
}

ORB_INLINE orb_vec3 orb_vec3_add(orb_vec3 lhs, orb_vec3 rhs) {
    return (orb_vec3){
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y,
        .z = lhs.z + rhs.z,
    };
}

ORB_INLINE orb_vec3 orb_vec3_sub(orb_vec3 lhs, orb_vec3 rhs) {
    return (orb_vec3){
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
        .z = lhs.z - rhs.z,
    };
}

ORB_INLINE orb_vec3 orb_vec3_scale(orb_vec3 lhs, f32 rhs) {
    return (orb_vec3){
        .x = lhs.x * rhs,
        .y = lhs.y * rhs,
        .z = lhs.z * rhs,
    };
}

/**
 * @brief dot product.
 */
ORB_INLINE f32 orb_vec3_dot(orb_vec3 lhs, orb_vec3 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

/**
 * @brief cross product.
 */
ORB_INLINE orb_vec3 orb_vec3_cross(orb_vec3 lhs, orb_vec3 rhs) {
    return (orb_vec3){
        .x = lhs.y * rhs.z - lhs.z * rhs.y,
        .y = lhs.z * rhs.x - lhs.x * rhs.z,
        .z = lhs.x * rhs.y - lhs.y * rhs.x,
    };
}

/**
 * @brief element-wise multiplication.
 */
ORB_INLINE orb_vec3 orb_vec3_hadamard_prod(orb_vec3 lhs, orb_vec3 rhs) {
    return (orb_vec3){
        .x = lhs.x * rhs.x,
        .y = lhs.y * rhs.y,
        .z = lhs.z * rhs.z,
    };
}

/**
 * @brief element-wise division.
 */
ORB_INLINE orb_vec3 orb_vec3_hadamard_div(orb_vec3 lhs, orb_vec3 rhs) {
    return (orb_vec3){
        .x = lhs.x / rhs.x,
        .y = lhs.y / rhs.y,
        .z = lhs.z / rhs.z,
    };
}

ORB_INLINE f32 orb_vec3_magnitude_sqr(orb_vec3 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

ORB_INLINE f32 orb_vec3_magnitude(orb_vec3 vec) { return orb_sqrt(orb_vec3_magnitude_sqr(vec)); }

/**
 * @brief normalize in place
 */
ORB_INLINE void orb_vec3_normalize(orb_vec3 *vec) {
    const f32 length = orb_vec3_magnitude(*vec);
    vec->x /= length;
    vec->y /= length;
    vec->z /= length;
}

/**
 * @brief create new normalized vector
 */
ORB_INLINE orb_vec3 orb_vec3_normalized(orb_vec3 vec) {
    orb_vec3_normalize(&vec);
    return vec;
}

/**
 * @brief returns true if all elements are equal within some tolerance; usually ORB_FLOAT_EPSILON
 */
ORB_INLINE b8 orb_vec3_equal(const orb_vec3 *lhs, const orb_vec3 *rhs, f32 tolerance) {
    if (orb_abs(lhs->x - rhs->x) > tolerance) {
        return false;
    }

    if (orb_abs(lhs->y - rhs->y) > tolerance) {
        return false;
    }

    if (orb_abs(lhs->z - rhs->z) > tolerance) {
        return false;
    }

    return true;
}

ORB_INLINE f32 orb_vec3_distance(orb_vec3 lhs, orb_vec3 rhs) {
    return orb_vec3_magnitude(orb_vec3_sub(lhs, rhs));
}

//
// vec4
//

ORB_INLINE orb_vec4 orb_vec4_new(f32 x, f32 y, f32 z, f32 w) {
    return (orb_vec4){
        .x = x,
        .y = y,
        .z = z,
        .w = w,
    };
}

ORB_INLINE orb_vec4 orb_vec4_zero() {
    return (orb_vec4){
        .x = 0,
        .y = 0,
        .z = 0,
        .w = 0,
    };
}

ORB_INLINE orb_vec4 orb_vec4_add(orb_vec4 lhs, orb_vec4 rhs) {
    return (orb_vec4){
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y,
        .z = lhs.z + rhs.z,
        .w = lhs.w + rhs.w,
    };
}

ORB_INLINE orb_vec4 orb_vec4_sub(orb_vec4 lhs, orb_vec4 rhs) {
    return (orb_vec4){
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
        .z = lhs.z - rhs.z,
        .w = lhs.w - rhs.w,
    };
}

ORB_INLINE orb_vec4 orb_vec4_scale(orb_vec4 lhs, f32 rhs) {
    return (orb_vec4){
        .x = lhs.x * rhs,
        .y = lhs.y * rhs,
        .z = lhs.z * rhs,
        .w = lhs.w * rhs,
    };
}

/**
 * @brief dot product.
 */
ORB_INLINE f32 orb_vec4_dot(orb_vec4 lhs, orb_vec4 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

/**
 * @brief element-wise multiplication.
 */
ORB_INLINE orb_vec4 orb_vec4_hadamard_prod(orb_vec4 lhs, orb_vec4 rhs) {
    return (orb_vec4){
        .x = lhs.x * rhs.x,
        .y = lhs.y * rhs.y,
        .z = lhs.z * rhs.z,
        .w = lhs.w * rhs.w,
    };
}

/**
 * @brief element-wise division.
 */
ORB_INLINE orb_vec4 orb_vec4_hadamard_div(orb_vec4 lhs, orb_vec4 rhs) {
    return (orb_vec4){
        .x = lhs.x / rhs.x,
        .y = lhs.y / rhs.y,
        .z = lhs.z / rhs.z,
        .w = lhs.w / rhs.w,
    };
}

ORB_INLINE f32 orb_vec4_magnitude_sqr(orb_vec4 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}

ORB_INLINE f32 orb_vec4_magnitude(orb_vec4 vec) { return orb_sqrt(orb_vec4_magnitude_sqr(vec)); }

/**
 * @brief normalize in place
 */
ORB_INLINE void orb_vec4_normalize(orb_vec4 *vec) {
    const f32 length = orb_vec4_magnitude(*vec);
    vec->x /= length;
    vec->y /= length;
    vec->z /= length;
    vec->w /= length;
}

/**
 * @brief create new normalized vector
 */
ORB_INLINE orb_vec4 orb_vec4_normalized(orb_vec4 vec) {
    orb_vec4_normalize(&vec);
    return vec;
}

/**
 * @brief returns true if all elements are equal within some tolerance; usually ORB_FLOAT_EPSILON
 */
ORB_INLINE b8 orb_vec4_equal(const orb_vec4 *lhs, const orb_vec4 *rhs, f32 tolerance) {
    if (orb_abs(lhs->x - rhs->x) > tolerance) {
        return false;
    }

    if (orb_abs(lhs->y - rhs->y) > tolerance) {
        return false;
    }

    if (orb_abs(lhs->z - rhs->z) > tolerance) {
        return false;
    }

    if (orb_abs(lhs->w - rhs->w) > tolerance) {
        return false;
    }

    return true;
}

ORB_INLINE f32 orb_vec4_distance(orb_vec4 lhs, orb_vec4 rhs) {
    return orb_vec4_magnitude(orb_vec4_sub(lhs, rhs));
}

//
// conversion between dimensions
//

ORB_INLINE orb_vec4 orb_vec4_from_vec3(orb_vec3 vec, f32 w) {
    return (orb_vec4){
        .x = vec.x,
        .y = vec.y,
        .z = vec.z,
        .w = w,
    };
}

ORB_INLINE orb_vec3 orb_vec3_from_vec4(orb_vec4 vec) {
    return (orb_vec3){
        .x = vec.x,
        .y = vec.y,
        .z = vec.z,
    };
}

//
// matrices
//

ORB_INLINE orb_mat4 orb_mat4_identity() {
    orb_mat4 mat = {0};

    mat.elements[0] = 1.0f;
    mat.elements[5] = 1.0f;
    mat.elements[10] = 1.0f;
    mat.elements[15] = 1.0f;

    return mat;
}

ORB_INLINE orb_mat4 orb_mat4_mul(const orb_mat4 *lhs, const orb_mat4 *rhs) {
    // no need to zero init because we write into all values
    orb_mat4 result;

    const f32 *left = lhs->elements;
    const f32 *right = rhs->elements;

    const u32 MATRIX_SIZE = 4;

    for (u32 row = 0; row < MATRIX_SIZE; ++row) {
        for (u32 col = 0; col < MATRIX_SIZE; ++col) {
            u32 rowOffset = row * MATRIX_SIZE;
            result.elements[col + rowOffset] = left[rowOffset + 0] * right[col + 0 * MATRIX_SIZE] +
                                               left[rowOffset + 1] * right[col + 1 * MATRIX_SIZE] +
                                               left[rowOffset + 2] * right[col + 2 * MATRIX_SIZE] +
                                               left[rowOffset + 3] * right[col + 3 * MATRIX_SIZE];
        }
    }

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip,
                                          f32 far_clip) {
    orb_mat4 result = orb_mat4_identity();

    f32 left_right = 1.0f / (left - right);
    f32 bottom_top = 1.0f / (bottom - top);
    f32 near_far = 1.0f / (near_clip - far_clip);

    result.elements[0] = -2.0f * left_right; // x
    result.elements[5] = -2.0f * bottom_top; // y
    result.elements[10] = -2.0f * near_far;  // z

    result.elements[12] = (left + right) * left_right;       // x
    result.elements[13] = (bottom + top) * bottom_top;       // y
    result.elements[14] = (near_clip + far_clip) * near_far; // z

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip,
                                         f32 far_clip) {
    orb_mat4 result = {0};

    f32 half_tan_fov = orb_tan(fov_radians * 0.5f);

    result.elements[0] = 1.0f / (aspect_ratio * half_tan_fov);
    result.elements[5] = 1.0f / half_tan_fov;
    result.elements[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    result.elements[11] = -1.0f;
    result.elements[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_look_at(orb_vec3 position, orb_vec3 target, orb_vec3 up) {
    orb_mat4 result = {0};

    ORB_DEBUG_ASSERT(orb_abs(orb_vec3_magnitude_sqr(up) - 1.0f) > ORB_FLOAT_EPSILON,
                     "up vector must be normalized");

    orb_vec3 z_axis = orb_vec3_normalized(orb_vec3_sub(target, position));
    orb_vec3 x_axis = orb_vec3_cross(z_axis, up);
    orb_vec3 y_axis = orb_vec3_cross(x_axis, z_axis);

    result.elements[0] = x_axis.x;
    result.elements[1] = y_axis.x;
    result.elements[2] = -z_axis.x;

    result.elements[4] = x_axis.y;
    result.elements[5] = y_axis.y;
    result.elements[6] = -z_axis.y;

    result.elements[8] = x_axis.z;
    result.elements[9] = y_axis.z;
    result.elements[10] = -z_axis.z;

    result.elements[12] = -orb_vec3_dot(x_axis, position);
    result.elements[13] = -orb_vec3_dot(y_axis, position);
    result.elements[14] = orb_vec3_dot(z_axis, position);
    result.elements[15] = 1.0f;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_transposed(const orb_mat4 *mat) {
    orb_mat4 result;

    result.elements[0] = mat->elements[0];
    result.elements[1] = mat->elements[4];
    result.elements[2] = mat->elements[8];
    result.elements[3] = mat->elements[12];
    result.elements[4] = mat->elements[1];
    result.elements[5] = mat->elements[5];
    result.elements[6] = mat->elements[9];
    result.elements[7] = mat->elements[13];
    result.elements[8] = mat->elements[2];
    result.elements[9] = mat->elements[6];
    result.elements[10] = mat->elements[10];
    result.elements[11] = mat->elements[14];
    result.elements[12] = mat->elements[3];
    result.elements[13] = mat->elements[7];
    result.elements[14] = mat->elements[11];
    result.elements[15] = mat->elements[15];

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_inverse(const orb_mat4 *mat) {
    orb_mat4 result;

    const f32 *m = mat->elements;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    f32 *r = result.elements;

    r[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    r[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    r[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    r[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 determinant = (m[0] * r[0] + m[4] * r[1] + m[8] * r[2] + m[12] * r[3]);
    ORB_DEBUG_ASSERT(orb_abs(determinant) > ORB_FLOAT_EPSILON,
                     "matrix is not invertible: determinant zero");

    f32 d = 1.0f / determinant;

    r[0] = d * r[0];
    r[1] = d * r[1];
    r[2] = d * r[2];
    r[3] = d * r[3];
    r[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    r[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    r[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    r[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    r[8] =
        d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    r[9] =
        d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    r[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    r[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    r[12] =
        d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    r[13] =
        d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    r[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    r[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_translation(orb_vec3 position) {
    orb_mat4 result = orb_mat4_identity();

    result.elements[12] = position.x;
    result.elements[13] = position.y;
    result.elements[14] = position.z;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_scale(orb_vec3 scale) {
    orb_mat4 result = orb_mat4_identity();

    result.elements[0] = scale.x;
    result.elements[5] = scale.y;
    result.elements[10] = scale.z;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_euler_rotation_x(f32 angle_radians) {
    orb_mat4 result = orb_mat4_identity();

    f32 cos = orb_cos(angle_radians);
    f32 sin = orb_sin(angle_radians);

    result.elements[5] = cos;
    result.elements[6] = sin;
    result.elements[9] = -sin;
    result.elements[10] = cos;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_euler_rotation_y(f32 angle_radians) {
    orb_mat4 result = orb_mat4_identity();

    f32 cos = orb_cos(angle_radians);
    f32 sin = orb_sin(angle_radians);

    result.elements[0] = cos;
    result.elements[2] = -sin;
    result.elements[8] = sin;
    result.elements[10] = cos;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_euler_rotation_z(f32 angle_radians) {
    orb_mat4 result = orb_mat4_identity();

    f32 cos = orb_cos(angle_radians);
    f32 sin = orb_sin(angle_radians);

    result.elements[0] = cos;
    result.elements[1] = sin;
    result.elements[4] = -sin;
    result.elements[5] = cos;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_euler_rotation_xyz(f32 x_radians, f32 y_radians, f32 z_radians) {
    orb_mat4 x = orb_mat4_from_euler_rotation_x(x_radians);
    orb_mat4 y = orb_mat4_from_euler_rotation_y(y_radians);
    orb_mat4 z = orb_mat4_from_euler_rotation_z(z_radians);

    orb_mat4 xy = orb_mat4_mul(&x, &y);
    orb_mat4 xyz = orb_mat4_mul(&xy, &z);

    return xyz;
}

/**
 * @brief Returns a forward vector relative to the provided matrix.
 */
ORB_INLINE orb_vec3 orb_mat4_forward(const orb_mat4 *matrix) {
    // -orb_vec3_from_vec4(matrix.cols[2]);
    orb_vec3 forward = {
        .x = -matrix->elements[2],
        .y = -matrix->elements[6],
        .z = -matrix->elements[10],
    };

    orb_vec3_normalize(&forward);

    return forward;
}

/**
 * @brief Returns a backward vector relative to the provided matrix.
 */
ORB_INLINE orb_vec3 orb_mat4_backward(const orb_mat4 *matrix) {
    // orb_vec3_from_vec4(matrix.cols[2]);
    orb_vec3 backward = {
        .x = matrix->elements[2],
        .y = matrix->elements[6],
        .z = matrix->elements[10],
    };

    orb_vec3_normalize(&backward);

    return backward;
}

/**
 * @brief Returns an upward vector relative to the provided matrix.
 */
ORB_INLINE orb_vec3 orb_mat4_up(const orb_mat4 *matrix) {
    // orb_vec3_from_vec4(matrix.cols[1]);
    orb_vec3 up = {
        .x = matrix->elements[1],
        .y = matrix->elements[5],
        .z = matrix->elements[9],
    };

    orb_vec3_normalize(&up);

    return up;
}

/**
 * @brief Returns a downward vector relative to the provided matrix.
 */
ORB_INLINE orb_vec3 orb_mat4_down(const orb_mat4 *matrix) {
    // -orb_vec3_from_vec4(matrix.cols[1]);
    orb_vec3 down = {
        .x = -matrix->elements[1],
        .y = -matrix->elements[5],
        .z = -matrix->elements[9],
    };

    orb_vec3_normalize(&down);

    return down;
}

/**
 * @brief Returns a left vector relative to the provided matrix.
 */
ORB_INLINE orb_vec3 orb_mat4_left(const orb_mat4 *matrix) {
    // -worb_vec3_from_vec4(matrix.cols[0]);
    orb_vec3 left = {
        .x = -matrix->elements[0],
        .y = -matrix->elements[4],
        .z = -matrix->elements[8],
    };

    orb_vec3_normalize(&left);

    return left;
}

/**
 * @brief Returns a right vector relative to the provided matrix.
 */
ORB_INLINE orb_vec3 orb_mat4_right(const orb_mat4 *matrix) {
    // orb_vec3_from_vec4(matrix.cols[0]);
    orb_vec3 right = {
        .x = matrix->elements[0],
        .y = matrix->elements[4],
        .z = matrix->elements[8],
    };

    orb_vec3_normalize(&right);

    return right;
}

//
// Quaternions
//

ORB_INLINE orb_quat orb_quat_identity() {
    return (orb_quat){
        .x = 0,
        .y = 0,
        .z = 0,
        .w = 0,
    };
}

ORB_INLINE f32 orb_quat_normal(const orb_quat *q) {
    return orb_sqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}

ORB_INLINE void orb_quat_normalize(orb_quat *q) {
    f32 normal = orb_quat_normal(q);
    q->x /= normal;
    q->y /= normal;
    q->z /= normal;
    q->w /= normal;
}

ORB_INLINE orb_quat orb_quat_normalized(orb_quat q) {
    orb_quat_normalize(&q);
    return q;
}

ORB_INLINE orb_quat orb_quat_conjugate(orb_quat q) {
    return (orb_quat){
        .x = -q.x,
        .y = -q.y,
        .z = -q.z,
        .w = q.w,
    };
}

ORB_INLINE orb_quat orb_quat_inverse(orb_quat q) {
    return orb_quat_normalized(orb_quat_conjugate(q));
}

ORB_INLINE orb_quat orb_quat_mul(const orb_quat *lhs, const orb_quat *rhs) {
    return (orb_quat){
        .x = rhs->x * lhs->w + rhs->y * lhs->z - rhs->z * lhs->y + rhs->w * lhs->x,
        .y = -rhs->x * lhs->z + rhs->y * lhs->w - rhs->z * lhs->x + rhs->w * lhs->y,
        .z = rhs->x * lhs->y - rhs->y * lhs->x + rhs->z * lhs->w + rhs->w * lhs->z,
        .w = -rhs->x * lhs->x - rhs->y * lhs->y - rhs->z * lhs->z + rhs->w * lhs->w,
    };
}

ORB_INLINE f32 orb_quat_dot(const orb_quat *lhs, const orb_quat *rhs) {
    return lhs->x * rhs->x + lhs->y * rhs->y + lhs->z * rhs->z + lhs->w * rhs->w;
}

ORB_INLINE orb_mat4 orb_mat4_from_quat(orb_quat q) {
    orb_mat4 result = {};
    f32 *o = (f32 *)&result.elements;

    orb_quat_normalize(&q);

    o[0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
    o[1] = 2.0f * q.x * q.y - 2.0f * q.z * q.w;
    o[2] = 2.0f * q.x * q.z + 2.0f * q.y * q.w;

    o[4] = 2.0f * q.x * q.y + 2.0f * q.z * q.w;
    o[5] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
    o[6] = 2.0f * q.y * q.z - 2.0f * q.x * q.w;

    o[8] = 2.0f * q.x * q.z - 2.0f * q.y * q.w;
    o[9] = 2.0f * q.y * q.z + 2.0f * q.x * q.w;
    o[10] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

    o[15] = 1.0f;

    return result;
}

ORB_INLINE orb_mat4 orb_mat4_from_quat_and_center(orb_quat q, orb_vec3 center) {
    orb_mat4 result = {};
    f32 *o = (f32 *)&result.elements;

    orb_quat_normalize(&q);

    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[15] = 1.0f;

    return result;
}

ORB_INLINE orb_quat orb_quat_from_axis_angle(orb_vec3 axis, f32 angle, b8 normalize) {
    const f32 half_angle = 0.5f * angle;
    f32 sin = orb_sin(half_angle);
    f32 cos = orb_cos(half_angle);

    orb_quat q = (orb_quat){
        .x = sin * axis.x,
        .y = sin * axis.y,
        .z = sin * axis.z,
        .w = cos,
    };

    if (normalize) {
        orb_quat_normalize(&q);
    }

    return q;
}

ORB_INLINE orb_quat orb_quat_slerp(orb_quat from, orb_quat to, f32 t) {
    orb_quat_normalize(&from);
    orb_quat_normalize(&to);

    f32 dot = orb_quat_dot(&from, &to);

    if (dot < 0.0f) {
        to.x = -to.x;
        to.y = -to.y;
        to.z = -to.z;
        to.w = -to.w;
        dot = -dot;
    }

    // quaternions that are very close to each other must be linearly interpolated
    // the acos operation will return 0 for very similar angles
    const f32 LERP_THRESHOLD = 0.9995f;
    if (dot > LERP_THRESHOLD) {
        return orb_quat_normalized((orb_quat){
            .x = ((to.x - from.x) * t),
            .y = ((to.y - from.y) * t),
            .z = ((to.z - from.z) * t),
            .w = ((to.w - from.w) * t),
        });
    }

    f32 theta_full = orb_acos(dot); // angle between input vectors
    f32 theta_t = theta_full * t;   // angle between from and result

    f32 sin_theta_full = orb_sin(theta_full);
    f32 sin_theta_t = orb_sin(theta_t);

    f32 s0 = orb_cos(theta_t) - dot * sin_theta_t / sin_theta_full;
    f32 s1 = sin_theta_t / sin_theta_full;

    return (orb_quat){
        .x = (from.x * s0) + (to.x * s1),
        .y = (from.y * s0) + (to.y * s1),
        .z = (from.z * s0) + (to.z * s1),
        .w = (from.w * s0) + (to.w * s1),
    };
}
