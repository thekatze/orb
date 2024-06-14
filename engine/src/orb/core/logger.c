#include "logger.h"

#include <stdarg.h>
// TODO: replace with platform layer call
#include <stdio.h>
#include <string.h>

b8 orb_logger_init() {
  // TODO: init log to file
  return TRUE;
}

void orb_logger_shutdown() {
  // TODO: close file handle, write queued entries
}

static const char *level_strings[6] = {
    "[FATAL]: ", "[ERROR]: ", "[ WARN]: ",
    "[ INFO]: ", "[DEBUG]: ", "[TRACE]: ",
};

void orb_log(log_level level, const char *message, ...) {
  // stack allocate, logger must be high performance
  const u32 max_message_len = 32 * 1024;
  char formatted_message[max_message_len];
  memset(formatted_message, 0, sizeof(formatted_message));

  va_list arg_ptr;
  va_start(arg_ptr, message);
  vsnprintf(formatted_message, max_message_len, message, arg_ptr);
  va_end(arg_ptr);

  char out_message[max_message_len];
  sprintf(out_message, "%s%s\n", level_strings[level], formatted_message);

  // b8 is_error = level < LOG_LEVEL_WARN;

  // TODO: platform-specific output
  printf("%s", out_message);
}
