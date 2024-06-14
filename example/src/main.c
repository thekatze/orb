#include <orb/core/application.h>

int main(void) {
  application_state app = {0};
  application_config config = {
      .name = "orb engine",
      .x = 100,
      .y = 100,
      .width = 1600,
      .height = 900,
  };

  if (!orb_application_create(&app, &config)) {
    return 1;
  }

  if (!orb_application_run(&app)) {
    return 1;
  }
}
