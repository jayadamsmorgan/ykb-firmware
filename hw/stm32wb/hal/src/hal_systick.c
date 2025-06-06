#include "hal_systick.h"

#include "hal_cortex.h"
#include "utils/utils.h"

systick_frequency systick_freq = SYSTICK_FREQ_1KHZ;
volatile uint32_t tick;

hal_err systick_init() {
    if ((uint32_t)systick_freq == 0U) {
        return ERR_SYSTICK_INIT_FREQ_INV;
    }
    if (SysTick_Config(SystemCoreClock / (1000U / (uint32_t)systick_freq)) !=
        0U) {
        return ERR_SYSTICK_INIT_FREQ_CONFIG;
    }
    cortex_nvic_set_priority(SysTick_IRQn, 0U, 0U);
    return OK;
}

uint32_t systick_get_tick() { return tick; }

void systick_delay(uint32_t ms) {
    while (ms) {
        if (READ_BIT(SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk)) {
            ms--;
            __WFI();
        }
    }
}

__weak void SysTick_Handler(void) { tick += (uint32_t)systick_freq; }
