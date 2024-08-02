#include <orb/core/logger.h>
#include <orb/platform/platform.h>

#include "test_runner.h"

#include "allocators/linear_allocator_tests.h"
#include "math/orb_math_tests.h"

i32 main() {
    test_runner_initialize();

    orb_math_register_tests();
    linear_allocator_register_tests();

    return test_runner_run() == 0 ? 0 : 1;
}
