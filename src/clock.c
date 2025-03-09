#include "clock.h"

#include "error_handler.h"
#include "hal/clock.h"
#include "hal/flash.h"
#include "hal/hal_err.h"
#include "logging.h"
#include "stm32wbxx.h"
#include "system_stm32wbxx.h"

// Use HSE and the PLL to get a 64MHz SYSCLK
inline hal_err setup_clock() {

    LOG_DEBUG("CLOCK: Setting up...");

    // Enable the 32MHz external oscillator
    clock_hse_enable();
    LOG_TRACE("CLOCK: HSE enabled.");

    // VCO input is 8MHz -> PLLM = 32000000 / 8000000 = 4
    // 24*8MHz = 192MHz -> PLLN = 24
    // f_Q = 48MHz -> PLLQ = 4
    // f_R = 64MHz -> PLLR = 3
    clock_pll_config_t pll_config;
    pll_config.source = CLOCK_PLL_SOURCE_HSE;
    pll_config.pllr = CLOCK_PLLR_3;
    pll_config.pllm = CLOCK_PLLM_4;
    pll_config.plln = CLOCK_PLLN_24;
    pll_config.pllq = CLOCK_PLLQ_4;
    clock_pll_config(&pll_config);
#ifdef DEBUG
    clock_pll_update_config(&pll_config);
#endif // DEBUG
    LOG_TRACE_EXPECTED("CLOCK: PLL Source:", pll_config.source,
                       CLOCK_PLL_SOURCE_HSE);
    LOG_TRACE_EXPECTED("CLOCK: PLLR:", pll_config.pllr, CLOCK_PLLR_3);
    LOG_TRACE_EXPECTED("CLOCK: PLLM:", pll_config.pllm, CLOCK_PLLM_4);
    LOG_TRACE_EXPECTED("CLOCK: PLLN:", pll_config.plln, CLOCK_PLLN_24);
    LOG_TRACE_EXPECTED("CLOCK: PLLQ:", pll_config.pllq, CLOCK_PLLQ_4);

    // Enable the PLL
    clock_pll_enable();
    LOG_TRACE("CLOCK: PLL enabled.");

    // Set divider for HCLK2 to 2 so f_HCLK2 = 32MHz
    clock_hclk2_set_prescaler(CLOCK_HCLK2_PRESC_DIV_2);
    LOG_TRACE_EXPECTED("CLOCK: HCLK2 prescaler:", clock_hclk2_get_prescaler(),
                       CLOCK_HCLK2_PRESC_DIV_2);

    // Set flash latency to 3 because SYSCLK > 54MHz
    flash_select_latency(FLASH_LATENCY_THREE_WAIT);
    LOG_TRACE_EXPECTED("FLASH: Latency:", flash_get_latency(),
                       FLASH_LATENCY_THREE_WAIT);

    // Select SYSCLK source
    clock_select_source(CLOCK_SOURCE_PLL);
    LOG_TRACE("CLOCK: System clock source is PLL.");

    // Select PLLQ as 48MHz source for USB and RNG
    clock_usb_rng_select_source(CLOCK_USB_RNG_SOURCE_PLLQ);
    LOG_TRACE_EXPECTED("CLOCK: USB & RNG clock source:",
                       clock_usb_rng_get_source(), CLOCK_USB_RNG_SOURCE_PLLQ);

    // Update `SystemCoreClock` variable
    SystemCoreClockUpdate();
    LOG_DEBUG_EXPECTED("CLOCK: SystemCoreClock:", SystemCoreClock, 64000000);

    LOG_INFO("CLOCK: Setup complete.");

    return OK;
}
