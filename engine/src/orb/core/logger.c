#include "logger.h"
#include "../platform/platform.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void orb_report_assertion_failure(const char *expression, const char *message, const char *file,
                                  i32 line, const char* function) {
    orb_log(LOG_LEVEL_FATAL, "%s:%d Assertion Failure in %s: %s: '%s'", file, line, function, expression, message);
}

typedef struct logger_system_state {
    b8 dummy_data;
} logger_system_state;

static logger_system_state *state;

b8 orb_logger_init(u64 *memory_requirement, void *memory) {
    *memory_requirement = sizeof(logger_system_state);
    if (memory == nullptr) {
        return true;
    }

    state = memory;

    // TODO: init log to file
    state->dummy_data = true;

    return true;
}

void orb_logger_shutdown() {
    // TODO: close file handle, write queued entries

    state = nullptr;
}

static const char *level_strings[6] = {
    "[FATAL]: ", "[ERROR]: ", "[ WARN]: ", "[ INFO]: ", "[DEBUG]: ", "[TRACE]: ",
};

void orb_log(orb_log_level level, const char *message, ...) {
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
