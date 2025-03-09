#ifndef LOGGING_H
#define LOGGING_H

#include "hal/hal_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LOG_TRACE(ARGS...) LOG(LOG_LEVEL_TRACE, ARGS)

#define LOG_DEBUG(ARGS...) LOG(LOG_LEVEL_DEBUG, ARGS)

#define LOG_INFO(ARGS...) LOG(LOG_LEVEL_INFO, ARGS)

#define LOG_ERROR(ARGS...) LOG(LOG_LEVEL_ERROR, ARGS)

#define LOG_CRITICAL(ARGS...) LOG(LOG_LEVEL_CRITICAL, ARGS)

#ifdef DEBUG

#define LOG_TRACE_EXPECTED(STR, VALUE, EXPECTED)                               \
    do {                                                                       \
        if (VALUE != EXPECTED) {                                               \
            LOG_ERROR(STR " %d. Expected: %d", VALUE, EXPECTED);               \
        } else {                                                               \
            LOG_TRACE(STR " %d", VALUE);                                       \
        }                                                                      \
    } while (0)

#define LOG_DEBUG_EXPECTED(STR, VALUE, EXPECTED)                               \
    do {                                                                       \
        if (VALUE != EXPECTED) {                                               \
            LOG_ERROR(STR " %d. Expected: %d", VALUE, EXPECTED);               \
        } else {                                                               \
            LOG_DEBUG(STR " %d", VALUE);                                       \
        }                                                                      \
    } while (0)

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

#define LOG_TRACE_EXPECTED(STR, VALUE, EXPECTED)
#define LOG_DEBUG_EXPECTED(STR, VALUE, EXPECTED)

#define LOG(LEVEL, ARGS...)

#define setup_logging() OK

#endif // DEBUG

#endif // LOGGING_H
