#include "core/application.h"
#include "core/logger.h"
#include "core/orb_memory.h"
#include "game_types.h"

[[nodiscard]]
extern b8 create_game(orb_game *out_game);

int main(void) {
  orb_memory_init();

  orb_game game_instance = {0};
  if (!create_game(&game_instance)) {
    ORB_FATAL("game could not be created");
    return -1;
  }

  // validate function pointers
  if (!game_instance.initialize || !game_instance.update ||
      !game_instance.render || !game_instance.on_resize) {
    ORB_FATAL("game function pointers are not assigned.");
    return -2;
  }

  if (!orb_application_create(&game_instance)) {
    return -3;
  }

  if (!orb_application_run()) {
    return -4;
  }

  orb_memory_shutdown();
}

