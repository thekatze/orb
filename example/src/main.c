#include <orb/core/asserts.h>
#include <orb/core/logger.h>
#include <orb/platform/platform.h>

int main(void) {
  ORB_TRACE("normal thing happened, we got PI: %f", 3.14f);
  ORB_DEBUG("we might be interested");
  ORB_INFO("window opened");
  ORB_WARN("swapchain out of date");
  ORB_ERROR("event loop broke");
  ORB_FATAL("computer dead");

  ORB_DEBUG_ASSERT(1 == 1, "math still works");

  return 0;
}
