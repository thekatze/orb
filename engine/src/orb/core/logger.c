#include "logger.h"
#include "../platform/platform.h"
#include "asserts.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void report_assertion_failure(const char *expression, const char *message,
                              const char *file, i32 line) {
  orb_log(LOG_LEVEL_FATAL, "%s:%d Assertion Failure: %s: '%s'", file, line,
          expression, message);
}

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

  if (level < LOG_LEVEL_WARN) {
    orb_platform_console_write_error(out_message, (u8)level);
  } else {
    orb_platform_console_write(out_message, (u8)level);
  }
}
