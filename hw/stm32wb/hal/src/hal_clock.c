#include "hal_clock.h"

#include "hal_bits.h"
#include "stm32wbxx.h"
#include "system_stm32wbxx.h"

#include <stdint.h>

// Global

static inline uint32_t clock_get_msi_frequency() {
    uint32_t msiRange = READ_BIT(RCC->CR, RCC_CR_MSIRANGE);
    if (msiRange > RCC_CR_MSIRANGE_11) {
        msiRange = RCC_CR_MSIRANGE_11;
    }
    msiRange = (msiRange & RCC_CR_MSIRANGE_Msk) >> RCC_CR_MSIRANGE_Pos;
    return MSIRangeTable[msiRange];
}

static inline clock_source clock_get_source() {
    return READ_BITS(RCC->CFGR, RCC_CFGR_SWS_Pos, BITMASK_2BIT);
}

static inline clock_pll_source clock_pll_get_source(void) {
    return READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC);
}

static inline bool clock_hse_div2_enabled() {
    return READ_BIT(RCC->CR, RCC_CR_HSEPRE);
}

uint32_t clock_get_hclk_frequency() {
    uint32_t sys_clock = clock_get_system_clock();
    uint32_t ahb_prescaler =
        READ_BITS(RCC->CFGR, RCC_CFGR_HPRE_Pos, BITMASK_4BIT);
    return sys_clock / AHBPrescTable[ahb_prescaler];
}

uint32_t clock_get_pclk1_frequency() {
    uint32_t hclk_freq = clock_get_hclk_frequency();
    uint32_t apb1_prescaler =
        READ_BITS(RCC->CFGR, RCC_CFGR_PPRE1_Pos, BITMASK_3BIT);
    return hclk_freq >> (APBPrescTable[apb1_prescaler] & 31U);
}

uint32_t clock_get_pclk2_frequency() {
    uint32_t hclk_freq = clock_get_hclk_frequency();
    uint32_t apb2_prescaler =
        READ_BITS(RCC->CFGR, RCC_CFGR_PPRE2_Pos, BITMASK_3BIT);
    return hclk_freq >> (APBPrescTable[apb2_prescaler] & 31U);
}

uint32_t clock_get_system_clock() {

    uint32_t pllinputfreq;

    const clock_source source = clock_get_source();

    switch (source) {

    case CLOCK_SOURCE_MSI:
        return clock_get_msi_frequency();

    case CLOCK_SOURCE_HSI16:
        return HSI_VALUE;

    case CLOCK_SOURCE_HSE:
        if (clock_hse_div2_enabled()) {
            return HSE_VALUE / 2U;
        }
        return HSE_VALUE;

    case CLOCK_SOURCE_PLL:
    default:
        switch (clock_pll_get_source()) {

        case CLOCK_PLL_SOURCE_HSI16:
            pllinputfreq = HSI_VALUE;
            break;

        case CLOCK_PLL_SOURCE_HSE:
            if (clock_hse_div2_enabled()) {
                pllinputfreq = HSE_VALUE / 2U;
            } else {
                pllinputfreq = HSE_VALUE;
            }
            break;

        case CLOCK_PLL_SOURCE_NONE:
        case CLOCK_PLL_SOURCE_MSI:
        default:
            pllinputfreq = clock_get_msi_frequency();
            break;
        }
        clock_pll_config_t config;
        clock_pll_update_config(&config);

        return pllinputfreq * config.plln / (config.pllm + 1U) /
               (config.pllr + 1U);
    }
}

void clock_hclk2_set_prescaler(clock_hclk2_prescaler presc) {
    MODIFY_BITS(RCC->EXTCFGR, RCC_EXTCFGR_C2HPRE_Pos, presc, BITMASK_4BIT);
}

clock_hclk2_prescaler clock_hclk2_get_prescaler() {
    return READ_BITS(RCC->EXTCFGR, RCC_EXTCFGR_C2HPRE_Pos, BITMASK_4BIT);
}

// HSE

void clock_hse_enable() {
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0) {
        // Wait for HSE to be ready
    }
}

// PLL

void clock_pll_update_config(clock_pll_config_t *config) {
    if (!config)
        return;

    if (READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQEN)) {
        config->pllq =
            READ_BITS(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_Pos, BITMASK_3BIT);
    } else {
        config->pllq = CLOCK_PLLQ_NONE;
    }

    if (READ_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN)) {
        config->pllr =
            READ_BITS(RCC->PLLCFGR, RCC_PLLCFGR_PLLR_Pos, BITMASK_3BIT);
    } else {
        config->pllr = CLOCK_PLLR_NONE;
    }

    config->plln = READ_BITS(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Pos, BITMASK_7BIT);
    config->pllm = READ_BITS(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_Pos, BITMASK_3BIT);
    config->source =
        READ_BITS(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Pos, BITMASK_2BIT);
}

void clock_pll_config(const clock_pll_config_t *config) {

    if (!config)
        return;

    uint32_t pll_cfgr = 0;

    if (config->pllq != CLOCK_PLLQ_NONE) {
        pll_cfgr |= RCC_PLLCFGR_PLLQEN | (config->pllq << RCC_PLLCFGR_PLLQ_Pos);
    }

    if (config->pllr != CLOCK_PLLR_NONE) {
        pll_cfgr |= RCC_PLLCFGR_PLLREN | (config->pllr << RCC_PLLCFGR_PLLR_Pos);
    }

    if (config->pllp != CLOCK_PLLP_NONE) {
        pll_cfgr |= RCC_PLLCFGR_PLLPEN | (config->pllp << RCC_PLLCFGR_PLLP_Pos);
    }

    pll_cfgr |= config->plln << RCC_PLLCFGR_PLLN_Pos;

    pll_cfgr |= config->pllm << RCC_PLLCFGR_PLLM_Pos;

    pll_cfgr |= config->source << RCC_PLLCFGR_PLLSRC_Pos;

    WRITE_REG(RCC->PLLCFGR, pll_cfgr);
}

void clock_pll_enable() {
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0) {
        // Wait for PLL to be ready
    }
}
