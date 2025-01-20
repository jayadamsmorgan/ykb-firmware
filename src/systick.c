#include "systick.h"

#include "clock.h"

volatile uint32_t s_ticks;

void systick_init(uint32_t ticks) {
    if ((ticks - 1) > 0xffffff)
        return; // Systick timer is 24bit
    SYSTICK->LOAD = ticks - 1;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = 7UL; // Enable systick
}

void systick_init_default() { systick_init(get_sys_freq() / 1000); }

void SysTick_Handler(void) { s_ticks++; }

void delay(unsigned ms) {
    uint32_t until = s_ticks + ms;
    while (s_ticks < until)
        (void)0;
}

// t: expiration time, prd: period, now: current time. Return true if expired
bool timer_expired(uint32_t *t, uint32_t prd) {
    if (s_ticks + prd < *t)
        *t = 0; // Time wrapped? Reset timer
    if (*t == 0)
        *t = s_ticks + prd; // First poll? Set expiration
    if (*t > s_ticks)
        return false; // Not expired yet, return
    *t =
        (s_ticks - *t) > prd ? s_ticks + prd : *t + prd; // Next expiration time
    return true;                                         // Expired, return true
}
