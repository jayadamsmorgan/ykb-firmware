#ifndef LOGGING_H
#define LOGGING_H

#include "hal/hal_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef DEBUG

#define LOG(VAR...)                                                            \
    do {                                                                       \
        printf("\r\nLOG: " VAR);                                               \
        fflush(stdout);                                                        \
    } while (0)

#define INFO(VAR...)                                                           \
    do {                                                                       \
        printf("\r\nINFO: " VAR);                                              \
        fflush(stdout);                                                        \
    } while (0)

#define TRACE(VAR...)                                                          \
    do {                                                                       \
        printf("\r\nTRACE: " VAR);                                             \
        fflush(stdout);                                                        \
    } while (0)

#define ERROR(VAR...)                                                          \
    do {                                                                       \
        printf("\r\nERROR: " VAR);                                             \
        fflush(stdout);                                                        \
    } while (0)

hal_err setup_logging();

#else // DEBUG

#define LOG(VAR...)
#define INFO(VAR...)
#define TRACE(VAR...)

#define setup_logging() OK

#endif // DEBUG

#endif // LOGGING_H
