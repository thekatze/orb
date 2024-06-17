#include "application.h"
#include "../game_types.h"
#include "../platform/platform.h"
#include "logger.h"

typedef struct application_state {
  b8 is_initialized;
  b8 is_running;
  b8 is_suspended;
  game *game_instance;
  platform_state platform;
  i16 width;
  i16 height;
  f64 last_tick;
} application_state;

static application_state app = {0};

[[nodiscard]]
ORB_API b8 orb_application_create(game *game_instance) {
  if (app.is_initialized)
    return FALSE;

  app.game_instance = game_instance;

  // initialize all subsystems
  orb_logger_init();

  application_config config = app.game_instance->app_config;
  if (!orb_platform_init(&app.platform, config.name, config.x, config.y,
                         config.width, config.height)) {

    ORB_FATAL("Could not initialize platform layer");
    return FALSE;
  }

  if (!app.game_instance->initialize(app.game_instance)) {
    ORB_FATAL("Game failed to initialize");
    return FALSE;
  }

  app.game_instance->on_resize(app.game_instance, config.width, config.height);

  app.is_initialized = TRUE;
  app.is_suspended = FALSE;

  return TRUE;
}

[[nodiscard]]
ORB_API b8 orb_application_run() {
  app.is_running = TRUE;

  while (orb_platform_events_pump(&app.platform)) {
    if (!app.is_suspended) {
      f32 delta = 0;

      if (!app.game_instance->update(app.game_instance, delta)) {
        ORB_FATAL("Update failed, shutting down");
        break;
      }

      if (!app.game_instance->render(app.game_instance, delta)) {
        ORB_FATAL("Render failed, shutting down");
        break;
      }
    }
  }

  app.is_running = FALSE;

  // done running, shutdown all systems
  orb_platform_shutdown(&app.platform);
  orb_logger_shutdown();

  return TRUE;
}
