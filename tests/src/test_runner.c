#include "test_runner.h"

#include <orb/containers/dynamic_array.h>
#include <orb/core/clock.h>
#include <orb/core/logger.h>

typedef struct registered_test {
    unit_test_fn execute;
    const char *description;
} registered_test;

static orb_dynamic_array tests;

void test_runner_initialize() { tests = orb_dynamic_array_create(registered_test); }

void test_runner_register_with_description(unit_test_fn fn, const char *description) {
    registered_test test = {
        .execute = fn,
        .description = description,
    };

    orb_dynamic_array_push(tests, test);
}

usize test_runner_run() {
    usize passed = 0;
    usize failed = 0;

    orb_clock total_time;
    orb_clock_start(&total_time);

    for (usize i = 0; i < tests.length; ++i) {
        registered_test test = ((registered_test *)tests.items)[i];

        orb_clock test_time;
        orb_clock_start(&test_time);
        u8 result = test.execute();
        orb_clock_update(&test_time);

        const char *status = "passed";
        orb_log_level log_level = LOG_LEVEL_INFO;

        if (result) {
            passed++;
        } else {
            failed++;
            status = "FAILED";
            log_level = LOG_LEVEL_ERROR;
        }

        orb_log(log_level, "Test %d of %d: %s %s in %.3fms", i + 1, tests.length, test.description,
                status, test_time.elapsed * 1000.0f);
    }

    orb_clock_update(&total_time);

    ORB_INFO("Test suite finished with %lld of %lld successful tests in %.3fms", passed,
             tests.length, total_time.elapsed * 1000.0f);

    return failed;
}
