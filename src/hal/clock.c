#include "hal/clock.h"
#include "hal/bits.h"
#include "stm32wbxx.h"

// Global

void clock_hclk2_set_prescaler(clock_hclk2_prescaler presc) {
    MODIFY_BITS(RCC->EXTCFGR, RCC_EXTCFGR_C2HPRE_Pos, presc, BITMASK_4BIT);
}

void clock_select_source(clock_source source) {
    MODIFY_BITS(RCC->CFGR, RCC_CFGR_SW_Pos, source, BITMASK_2BIT);
    while (READ_BITS(RCC->CFGR, RCC_CFGR_SWS_Pos, BITMASK_2BIT) != source) {
    }
}

void clock_usb_rng_select_source(clock_usb_rng_source source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_CLK48SEL_Pos, source, BITMASK_2BIT);
}

// HSE

void clock_hse_enable() {
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0) {
        // Wait for HSE to be ready
    }
}

// PLL

void clock_pll_config(clock_pllm pllm, clock_plln plln, clock_pllq pllq,
                      clock_pllr pllr, clock_pll_source source) {

    uint32_t pll_cfgr = 0;

    if (pllq != CLOCK_PLLQ_NONE) {
        pll_cfgr |= RCC_PLLCFGR_PLLQEN | pllq << RCC_PLLCFGR_PLLQ_Pos;
    }

    if (pllr != CLOCK_PLLR_NONE) {
        pll_cfgr |= RCC_PLLCFGR_PLLREN | pllr << RCC_PLLCFGR_PLLR_Pos;
    }

    pll_cfgr |= plln << RCC_PLLCFGR_PLLN_Pos;

    pll_cfgr |= pllm << RCC_PLLCFGR_PLLM_Pos;

    pll_cfgr |= source << RCC_PLLCFGR_PLLSRC_Pos;

    WRITE_REG(RCC->PLLCFGR, pll_cfgr);
}

void clock_pll_enable() {
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0) {
        // Wait for PLL to be ready
    }
}
