#pragma once

#include "../../../core/types.h"

// forward declare, because we need to first include the vulkan header with a
// specific platform dependent define to allow surface creation
struct orb_vulkan_context;

b8 orb_vulkan_platform_surface_init(struct orb_vulkan_context *context);
