#include "orb_math_tests.h"
#include "../expect.h"
#include "../test_runner.h"

#include <orb/core/types.h>
#include <orb/math/orb_math.h>

b8 mat4_mul_should_work_correctly() {
    orb_mat4 lhs = {
        .elements =
            {
                1, 2, 3, 4,     //
                5, 6, 7, 8,     //
                9, 10, 11, 12,  //
                13, 14, 15, 16, //
            },
    };

    orb_mat4 rhs = {
        .elements =
            {
                1, 10, 9, 13,  //
                2, 20, 10, 14, //
                3, 30, 11, 2,  //
                4, 40, 12, 16, //
            },
    };

    orb_mat4 expected = {
        .elements =
            {
                30, 300, 110, 111,   //
                70, 700, 278, 291,   //
                110, 1100, 446, 471, //
                150, 1500, 614, 651, //
            },
    };

    orb_mat4 result = orb_mat4_mul(&lhs, &rhs);

    const usize MATRIX_SIZE = 4;
    for (usize i = 0; i < MATRIX_SIZE; ++i) {
        for (usize j = 0; j < MATRIX_SIZE; ++j) {
            usize i_j = j + i * MATRIX_SIZE;
            expect_float_eq(expected.elements[i_j], result.elements[i_j], ORB_FLOAT_EPSILON);
        }
    }

    return true;
}

void orb_math_register_tests() {
    // TODO: validate vectors when implementing SIMD
    test_runner_register(mat4_mul_should_work_correctly);
}
