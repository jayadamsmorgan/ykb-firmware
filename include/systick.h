#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdbool.h>
#include <stdint.h>

#define SYSTICK ((struct systick *)(0xE000E010UL))

struct systick {
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};

void systick_init(uint32_t ticks);
void systick_init_default();

void SysTick_Handler(void);

void delay(unsigned ms);

bool timer_expired(uint32_t *t, uint32_t prd);

#endif // SYSTICK_H
