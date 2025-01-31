#include "clock.h"

#include "error_handler.h"
#include "hal/clock.h"
#include "hal/flash.h"
#include "hal/hal_err.h"
#include "stm32wbxx.h"
#include "system_stm32wbxx.h"

// Use HSE and the PLL to get a 64MHz SYSCLK
inline hal_err setup_clock() {

    // Enable the 32MHz external oscillator
    clock_hse_enable();

    // VCO input is 8MHz -> PLLM = 32000000 / 8000000 = 4
    // 24*8MHz = 192MHz -> PLLN = 24
    // f_Q = 48MHz -> PLLQ = 4
    // f_R = 64MHz -> PLLR = 3
    clock_pll_config(CLOCK_PLLM_4, CLOCK_PLLN_24, CLOCK_PLLQ_4, CLOCK_PLLR_3,
                     CLOCK_PLL_SOURCE_HSE);

    // Enable the PLL
    clock_pll_enable();

    // Set divider for HCLK2 to 2 so f_HCLK2 = 32MHz
    clock_hclk2_set_prescaler(CLOCK_HCLK2_PRESC_DIV_2);

    // Set flash latency to 3 because SYSCLK > 54MHz
    flash_select_latency(FLASH_LATENCY_THREE_WAIT);

    // Select SYSCLK source
    clock_select_source(CLOCK_SOURCE_PLL);

    // Select PLLQ as 48MHz source for USB and RNG
    clock_usb_rng_select_source(CLOCK_USB_RNG_SOURCE_PLLQ);

    // Update `SystemCoreClock` variable
    SystemCoreClockUpdate();

    if (SystemCoreClock != 64000000) {
        return ERR_INV_SYSCLOCK;
    }

    return OK;
}
