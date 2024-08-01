#include <orb/core/logger.h>
#include <orb/platform/platform.h>

#include "./allocators/linear_allocator_tests.h"
#include "test_runner.h"

i32 main() {
    test_runner_initialize();

    linear_allocator_register_tests();

    return test_runner_run() == 0 ? 0 : 1;
}
