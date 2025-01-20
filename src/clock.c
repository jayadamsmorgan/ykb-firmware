#include "clock.h"

#include "rcc.h"
#include "system.h"

unsigned system_clock_freq = 16000000;

unsigned get_sys_freq() { return system_clock_freq; }

void select_clock_source(CLOCK_SOURCE source) {
    switch (source) {
    case HSE:
        *RCC_CR_REG &= ~(BIT(8));
        *RCC_CR_REG |= BIT(16);
        system_clock_freq = 64000000;
        break;
    case HSI:
        *RCC_CR_REG &= ~(BIT(16));
        *RCC_CR_REG |= BIT(8);
        system_clock_freq = 16000000;
        break;
    }
}
