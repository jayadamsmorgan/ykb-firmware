#ifndef LOGGING_H
#define LOGGING_H

#include "hal/hal_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef __NO_ASSERT

#define ASSERT_EXPR(X)                                                         \
    ((X) ? (true)                                                              \
         : LOG_CRITICAL("%s,%d: Assertion '%s' failed.", __FILE__, __LINE__,   \
                        #X),                                                   \
     false)

#define ASSERT(X)                                                              \
    do {                                                                       \
        if (!(X)) {                                                            \
            LOG_CRITICAL("%s,%d: Assertion '%s' failed.", __FILE__, __LINE__,  \
                         #X);                                                  \
        }                                                                      \
    } while (0)

#else

#define ASSERT_EXPR(X)
#define ASSERT(X)

#endif // __NO_ASSERT

#define LOG_TRACE(ARGS...) LOG(LOG_LEVEL_TRACE, ARGS)

#define LOG_DEBUG(ARGS...) LOG(LOG_LEVEL_DEBUG, ARGS)

#define LOG_INFO(ARGS...) LOG(LOG_LEVEL_INFO, ARGS)

#define LOG_ERROR(ARGS...) LOG(LOG_LEVEL_ERROR, ARGS)

#define LOG_CRITICAL(ARGS...) LOG(LOG_LEVEL_CRITICAL, ARGS)

#ifdef DEBUG

#define LOG(LEVEL, ARGS...) _log(LEVEL, ARGS)

typedef enum {
    LOG_LEVEL_TRACE = 0U,
    LOG_LEVEL_DEBUG = 1U,
    LOG_LEVEL_INFO = 2U,
    LOG_LEVEL_ERROR = 3U,
    LOG_LEVEL_CRITICAL = 4U,
} log_level;

void _log(log_level level, const char *format, ...);

hal_err setup_logging();

#else // DEBUG

#define LOG(LEVEL, ARGS...)

#define setup_logging() OK

#endif // DEBUG

#endif // LOGGING_H
