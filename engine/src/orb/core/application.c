#include "application.h"
#include "logger.h"

ORB_API b8 orb_application_create(application_state *app,
                                  application_config *config) {
  if (app->is_initialized)
    return FALSE;

  // initialize all subsystems
  orb_logger_init();

  if (!orb_platform_init(config->name, config->x, config->y, config->width,
                         config->height)) {
    ORB_FATAL("Could not initialize platform layer");
    return FALSE;
  }

  app->is_initialized = TRUE;
  app->is_suspended = FALSE;

  return TRUE;
}

ORB_API b8 orb_application_run(application_state *app) {
  app->is_running = TRUE;

  while (orb_platform_events_pump()) {
  }

  app->is_running = FALSE;

  // done running, shutdown all systems
  orb_platform_shutdown();
  orb_logger_shutdown();

  return TRUE;
}
