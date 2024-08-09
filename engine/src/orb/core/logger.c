#include "logger.h"
#include "../platform/filesystem.h"
#include "../platform/platform.h"
#include "orb_memory.h"
#include "orb_string.h"

#include <stdarg.h>

void orb_report_assertion_failure(const char *expression, const char *message, const char *file,
                                  i32 line, const char *function) {
    orb_log(LOG_LEVEL_FATAL, "%s:%d Assertion Failure in %s: %s: '%s'", file, line, function,
            expression, message);
}

typedef struct logger_system_state {
    orb_file_handle log_file;
} logger_system_state;

static logger_system_state *state;

b8 orb_logger_init(u64 *memory_requirement, void *memory) {
    *memory_requirement = sizeof(logger_system_state);
    if (memory == nullptr) {
        return true;
    }

    state = memory;

    if (!orb_filesystem_file_open("latest.log", FILE_MODE_WRITE_FLAG, false,
                                  &state->log_file)) {
        orb_platform_console_write_error("[ERROR]: Unable to open logfile.", LOG_LEVEL_ERROR);
        return false;
    }

    return true;
}

void orb_logger_shutdown() {
    orb_filesystem_file_close(&state->log_file);
    state = nullptr;
}

static const char *level_strings[6] = {
    "[FATAL]: ", "[ERROR]: ", "[ WARN]: ", "[ INFO]: ", "[DEBUG]: ", "[TRACE]: ",
};

void orb_log(orb_log_level level, const char *message, ...) {
    // stack allocate, logger must be high performance
    const u32 max_message_len = 32 * 1024;
    char formatted_message[max_message_len];
    orb_memory_zero(formatted_message, sizeof(formatted_message));

    va_list arg_ptr;
    va_start(arg_ptr, message);
    orb_string_format_v(formatted_message, message, arg_ptr);
    va_end(arg_ptr);

    char out_message[max_message_len];
    orb_string_format(out_message, "%s%s\n", level_strings[level], formatted_message);

    usize message_length = orb_string_length(out_message);
    usize written = 0;
    if (!orb_filesystem_file_append_bytes(&state->log_file, message_length, out_message,
                                          &written)) {
        orb_platform_console_write_error("[ERROR]: Failed to write to logfile.", LOG_LEVEL_ERROR);
    }

    if (level < LOG_LEVEL_WARN) {
        orb_platform_console_write_error(out_message, (u8)level);
    } else {
        orb_platform_console_write(out_message, (u8)level);
    }
}
