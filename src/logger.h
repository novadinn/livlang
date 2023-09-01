#ifndef LOGGER_H
#define LOGGER_H

#include "defines.h"

#include <stdlib.h>

typedef enum LogLevel {
  LOG_LEVEL_FATAL,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARN,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_TRACE,
} LogLevel;

void logger_log_output(LogLevel level, const char *message, ...);

#define V_FATAL(message, ...)                                                  \
  logger_log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);                  \
  exit(1);
#define V_ERROR(message, ...)                                                  \
  logger_log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);                  \
  exit(1);
#define V_WARN(message, ...)                                                   \
  logger_log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#define V_INFO(message, ...)                                                   \
  logger_log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);

#ifndef NDEBUG
#define V_DEBUG(message, ...)                                                  \
  logger_log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define V_DEBUG(message, ...)
#endif

#ifndef NDEBUG
#define V_TRACE(message, ...)                                                  \
  logger_log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define V_TRACE(message, ...)
#endif

#ifndef NDEBUG
#if _MSC_VER
#include <intrin.h>
#define debug_break() __debugbreak()
#else
#define debug_break() __builtin_trap()
#endif

void logger_report_assertion_failure(const char *expression,
                                     const char *message, const char *file,
                                     i32 line);

#define V_ASSERT(expr)                                                         \
  {                                                                            \
    if (expr) {                                                                \
    } else {                                                                   \
      logger_report_assertion_failure(#expr, "", __FILE__, __LINE__);          \
      debug_break();                                                           \
    }                                                                          \
  }

#define V_ASSERT_MSG(expr, message)                                            \
  {                                                                            \
    if (expr) {                                                                \
    } else {                                                                   \
      logger_report_assertion_failure(#expr, message, __FILE__, __LINE__);     \
      debug_break();                                                           \
    }                                                                          \
  }

#else
#define V_ASSERT(expr)
#define V_ASSERT_MSG(expr, message)
#define V_ASSERT_DEBUG(expr)
#endif

#endif // LOGGER_H