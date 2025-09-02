#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

#include "hal_clock.h"
#include "hal_err.h"

typedef enum {
    SYSTICK_FREQ_10HZ = 100U,
    SYSTICK_FREQ_100HZ = 10U,
    SYSTICK_FREQ_1KHZ = 1U,
} systick_frequency;

// Has to be called after clock setup
hal_err systick_init();

void systick_delay(uint32_t ms);

uint32_t systick_get_tick();

static inline void systick_suspend() {
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}

static inline void systick_resume() {
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}

#endif // HAL_SYSTICK_H
