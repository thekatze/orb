#include <orb/core/logger.h>
#include <orb/platform/platform.h>

#include "test_runner.h"

#include "allocators/linear_allocator_tests.h"
#include "math/orb_math_tests.h"

i32 main() {
    usize logger_memory_requirement;
    orb_logger_init(&logger_memory_requirement, nullptr);

    void *logger_memory = orb_platform_allocate(logger_memory_requirement, false);
    orb_logger_init(&logger_memory_requirement, logger_memory);

    ORB_DEBUG("Initializing test runner");
    test_runner_initialize();

    orb_math_register_tests();
    linear_allocator_register_tests();

    ORB_DEBUG("Starting test run");
    bool result = test_runner_run() == 0 ? 0 : 1;

    orb_logger_shutdown();
    orb_platform_free(logger_memory, false);

    return result;
}
