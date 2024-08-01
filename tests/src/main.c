#include "test_runner.h"
#include "expect.h"
#include <orb/core/logger.h>
#include <orb/platform/platform.h>

b8 test_runner_test() {
    expect(true);
    return true;
}

b8 test_runner_test2() {
    expect_false(true);
    return true;
}

i32 main() {
    ORB_INFO("Hello World!");

    test_runner_initialize();
    test_runner_register(test_runner_test);
    test_runner_register(test_runner_test2);
    return test_runner_run() ? 1 : 0;
}
