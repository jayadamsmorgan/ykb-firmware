#include "clock.h"

#include "hal_clock.h"
#include "hal_flash.h"

#include "logging.h"

// Use HSE and the PLL to get a 64MHz SYSCLK
inline hal_err setup_clock() {

    LOG_DEBUG("Setting up...");

    // Enable the 32MHz external oscillator
    clock_hse_enable();
    LOG_TRACE("HSE enabled.");

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

    clock_pll_update_config(&pll_config);
    ASSERT(pll_config.source == CLOCK_PLL_SOURCE_HSE);
    ASSERT(pll_config.pllr == CLOCK_PLLR_3);
    ASSERT(pll_config.pllm == CLOCK_PLLM_4);
    ASSERT(pll_config.plln == CLOCK_PLLN_24);
    ASSERT(pll_config.pllq == CLOCK_PLLQ_4);

    // Enable the PLL
    clock_pll_enable();
    LOG_TRACE("PLL enabled.");

    // Set divider for HCLK2 to 2 so f_HCLK2 = 32MHz
    clock_hclk2_set_prescaler(CLOCK_HCLK2_PRESC_DIV_2);
    ASSERT(clock_hclk2_get_prescaler() == CLOCK_HCLK2_PRESC_DIV_2);

    // Set flash latency to 3 because SYSCLK > 54MHz
    flash_select_latency(FLASH_LATENCY_THREE_WAIT);
    ASSERT(flash_get_latency() == FLASH_LATENCY_THREE_WAIT);

    // Select SYSCLK source
    clock_select_source(CLOCK_SOURCE_PLL);
    LOG_TRACE("System clock source is PLL.");

    // Select PLLQ as 48MHz source for USB and RNG
    clock_usb_rng_select_source(CLOCK_USB_RNG_SOURCE_PLLQ);
    ASSERT(clock_usb_rng_get_source() == CLOCK_USB_RNG_SOURCE_PLLQ);

    // Update `SystemCoreClock` variable
    SystemCoreClockUpdate();
    ASSERT(SystemCoreClock == 64000000);

    LOG_INFO("Setup complete.");

    return OK;
}
