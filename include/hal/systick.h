#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

#include "hal/clock.h"
#include "hal/hal_err.h"

typedef enum {
    SYSTICK_FREQ_10HZ = 100U,
    SYSTICK_FREQ_100HZ = 10U,
    SYSTICK_FREQ_1KHZ = 1U,
} systick_frequency;

// Has to be called after clock setup
hal_err systick_init();

void systick_delay(uint32_t ms);

uint32_t systick_get_tick();

#endif // HAL_SYSTICK_H
