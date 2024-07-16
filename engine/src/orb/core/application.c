#include "application.h"
#include "../game_types.h"
#include "../platform/platform.h"
#include "../renderer/renderer_frontend.h"
#include "clock.h"
#include "event.h"
#include "input.h"
#include "logger.h"

typedef struct application_state {
  b8 is_initialized;
  b8 is_running;
  b8 is_suspended;
  orb_game *game_instance;
  orb_platform_state platform;
  i16 width;
  i16 height;
  orb_clock clock;
  f64 last_frame_timestamp;
} application_state;

static application_state app = {0};

[[nodiscard]]
ORB_API b8 orb_application_create(orb_game *game_instance) {
  if (app.is_initialized)
    return FALSE;

  app.game_instance = game_instance;

  // initialize all subsystems
  orb_logger_init();

  orb_application_config config = app.game_instance->app_config;
  if (!orb_platform_init(&app.platform, config.name, config.x, config.y,
                         config.width, config.height)) {

    ORB_FATAL("Could not initialize platform layer");
    return FALSE;
  }

  if (!orb_event_init()) {
    ORB_FATAL("Could not initialize event system");
    return FALSE;
  }

  if (!orb_input_init()) {
    ORB_FATAL("Could not initialize event system");
    return FALSE;
  }

  if (!orb_renderer_init(app.game_instance->app_config.name, &app.platform)) {
    ORB_FATAL("Could not initialize renderer");
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

typedef b8 (*orb_event_handler_fn)(event_code code, void *sender,
                                   void *listener, orb_event_context context);

b8 orb_shutdown(event_code code, void *sender, void *listener,
                orb_event_context context) {
  (void)code;
  (void)sender;
  (void)listener;
  (void)context;

  app.is_running = false;
  ORB_INFO("Received ORB_EVENT_APPLICATION_QUIT, shutting down.");

  return TRUE;
}

[[nodiscard]]
ORB_API b8 orb_application_run() {
  app.is_running = TRUE;

  orb_event_add_listener(ORB_EVENT_APPLICATION_QUIT, nullptr, orb_shutdown);

  orb_clock_start(&app.clock);
  orb_clock_update(&app.clock);
  app.last_frame_timestamp = app.clock.elapsed;

  const f64 target_frame_seconds = 1.0 / 60.0;

  while (orb_platform_events_pump(&app.platform) && app.is_running) {
    if (app.is_suspended)
      continue;

    orb_clock_update(&app.clock);
    f64 current_frame_timestamp = app.clock.elapsed;
    f32 delta = (f32)(current_frame_timestamp - app.last_frame_timestamp);
    f64 frame_start_time = orb_platform_time_now();

    orb_input_update();

    if (!app.game_instance->update(app.game_instance, delta)) {
      ORB_FATAL("Update failed, shutting down");
      break;
    }

    if (!app.game_instance->render(app.game_instance, delta)) {
      ORB_FATAL("Render failed, shutting down");
      break;
    }

    orb_render_packet packet = {
        .delta_time = delta,
    };

    if (!orb_renderer_draw_frame(&packet)) {
      ORB_FATAL("Internal Render failed, shutting down");
      break;
    }

    f64 frame_elapsed_time = orb_platform_time_now() - frame_start_time;
    f64 remaining_seconds_in_frame = target_frame_seconds - frame_elapsed_time;

    if (remaining_seconds_in_frame > 0.001) {
      u64 sleep_ms = (u64)(remaining_seconds_in_frame * 1000);
      orb_platform_time_sleep(sleep_ms - 1);
    }

    app.last_frame_timestamp = current_frame_timestamp;
  }

  app.is_running = FALSE;

  // done running, shutdown all systems
  orb_renderer_shutdown();
  orb_input_shutdown();
  orb_event_shutdown();
  orb_platform_shutdown(&app.platform);
  orb_logger_shutdown();

  return TRUE;
}
