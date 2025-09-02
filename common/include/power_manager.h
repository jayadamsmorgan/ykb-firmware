#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "utils/utils.h"

typedef enum {
    POWER_MANAGER_MODE_SLEEP = 0U,
    POWER_MANAGER_MODE_STOP = 1U,
    POWER_MANAGER_MODE_OFF = 2U,
} power_manager_mode;

typedef enum {
    POWER_MANAGER_STATE_LPM_ENABLE = 0U,
    POWER_MANAGER_STATE_LPM_DISABLE = 1U,
} power_manager_state;

typedef uint8_t power_manager_pid;

void power_manager_init();

#endif // POWER_MANAGER_H
