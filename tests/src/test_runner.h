#pragma once

#include <orb/core/types.h>

typedef b8 (*unit_test_fn)();

void test_runner_initialize();
void test_runner_register_with_description(unit_test_fn fn, const char *description);

[[nodiscard]]
usize test_runner_run();

#define test_runner_register(fn) test_runner_register_with_description(fn, #fn)
