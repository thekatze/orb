#include "linear_allocator_tests.h"
#include "../expect.h"
#include "../test_runner.h"

#include <orb/allocators/linear_allocator.h>
#include <orb/core/orb_memory.h>
#include <orb/core/types.h>
#include <orb/platform/platform.h>

b8 linear_allocator_should_allocate_linearly() {
    const usize bytes = 1024;

    u8 *memory = orb_platform_allocate(sizeof(u8) * bytes, false);
    orb_linear_allocator allocator;
    orb_linear_allocator_create_view(bytes, memory, &allocator);

    expect_eq(0, allocator.allocated);
    expect_neq(0, allocator.memory);
    expect_eq(bytes, allocator.total_size);

    const usize first_allocation_size = 64;
    u8 *first_allocation = orb_linear_allocator_allocate(&allocator, first_allocation_size);

    expect_eq(memory, first_allocation);
    expect_eq(first_allocation_size, allocator.allocated);

    const usize second_allocation_size = 128;
    u8 *second_allocation = orb_linear_allocator_allocate(&allocator, second_allocation_size);

    expect_eq(memory + first_allocation_size, second_allocation);
    expect_eq(first_allocation_size + second_allocation_size, allocator.allocated);

    orb_platform_free(memory, false);

    return true;
}

void linear_allocator_register_tests() {
    test_runner_register(linear_allocator_should_allocate_linearly);
}
