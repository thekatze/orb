#pragma once

#include "types.h"

#define ORB_LOG_WARN 1
#define ORB_LOG_INFO 1
#define ORB_LOG_DEBUG 1
#define ORB_LOG_TRACE 1

#if ORB_RELEASE == 1
#define ORB_LOG_DEBUG 0;
#define ORB_LOG_TRACE 0;
#endif

typedef enum log_level {
  LOG_LEVEL_FATAL = 0,
  LOG_LEVEL_ERROR = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_INFO = 3,
  LOG_LEVEL_DEBUG = 4,
  LOG_LEVEL_TRACE = 5,
} log_level;

b8 orb_logger_init();
void orb_logger_shutdown();

ORB_API void orb_log(log_level level, const char *message, ...);

#define ORB_FATAL(message, ...)                                                \
  orb_log(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#define ORB_ERROR(message, ...)                                                \
  orb_log(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#if ORB_LOG_WARN == 1
#define ORB_WARN(message, ...) orb_log(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define ORB_WARN(message, ...)
#endif

#if ORB_LOG_INFO == 1
#define ORB_INFO(message, ...) orb_log(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define ORB_INFO(message, ...)
#endif

#if ORB_LOG_DEBUG == 1
#define ORB_DEBUG(message, ...)                                                \
  orb_log(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define ORB_DEBUG(message, ...)
#endif

#if ORB_LOG_TRACE == 1
#define ORB_TRACE(message, ...)                                                \
  orb_log(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define ORB_TRACE(message, ...)
#endif
