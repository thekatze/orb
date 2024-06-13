#pragma once

#define ORB_LOG_WARN 1;
#define ORB_LOG_INFO 1;
#define ORB_LOG_DEBUG 1;
#define ORB_LOG_TRACE 1;

#if ORB_RELEASE == 1
#define ORB_LOG_DEBUG 0;
#define ORB_LOG_TRACE 0;
#endif

typedef enum log_level {
  FATAL,
  ERROR,
  WARN,
  INFO,
  DEBUG,
  TRACE,
} log_level;

void initialize_logger();
void shutdown_logger();
