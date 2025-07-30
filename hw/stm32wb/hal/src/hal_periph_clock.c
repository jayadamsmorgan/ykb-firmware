#include "hal_periph_clock.h"

#include "hal_power.h"
#include "hal_systick.h"

hal_err periph_clock_config_pll_sai1(periph_clock_pllsai1_t pll_sai1) {
    uint32_t tick_start = systick_get_tick();

    // Disable PLLSAI1
    CLEAR_BIT(RCC->CR, RCC_CR_PLLSAI1ON);

    // Wait until off
    while (READ_BIT(RCC->CR, RCC_CR_PLLSAI1RDY)) {
        if (systick_get_tick() - tick_start >=
            PERIPHCLK_PLLSAI1_CONFIG_TIMEOUT) {
            return ERR_PERIPHCLK_CONFIGPLLSAI1_OFF_TIMEOUT;
        }
    }

    // N
    MODIFY_BITS(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLN_Pos, pll_sai1.plln,
                BITMASK_7BIT);

    // P
    if (pll_sai1.pllp != CLOCK_PLLP_NONE) {
        SET_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLPEN);
        MODIFY_BITS(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLP_Pos, pll_sai1.pllp,
                    BITMASK_5BIT);
    } else {
        CLEAR_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLPEN);
    }

    // Q
    if (pll_sai1.pllq != CLOCK_PLLQ_NONE) {
        SET_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLQEN);
        MODIFY_BITS(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLQ_Pos, pll_sai1.pllq,
                    BITMASK_3BIT);
    } else {
        CLEAR_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLQEN);
    }

    // R
    if (pll_sai1.pllr != CLOCK_PLLR_NONE) {
        SET_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLREN);
        MODIFY_BITS(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLR_Pos, pll_sai1.pllr,
                    BITMASK_3BIT);
    } else {
        CLEAR_BIT(RCC->PLLSAI1CFGR, RCC_PLLSAI1CFGR_PLLREN);
    }

    tick_start = systick_get_tick();

    // Wait until on
    while (READ_BIT(RCC->CR, RCC_CR_PLLSAI1RDY) == 0) {
        if (systick_get_tick() - tick_start >=
            PERIPHCLK_PLLSAI1_CONFIG_TIMEOUT) {
            return ERR_PERIPHCLK_CONFIGPLLSAI1_ON_TIMEOUT;
        }
    }

    return OK;
}

hal_err periph_clock_select_rtc_source(periph_clock_rtc_source rtc_source) {

    periph_clock_rtc_source current = periph_clock_get_rtc_source();

    if (current == rtc_source)
        return OK;

    hal_power_enable_backup_access();

    if (current == PERIPHCLK_RTC_NONE) {
        MODIFY_BITS(RCC->BDCR, RCC_BDCR_RTCSEL_Pos, rtc_source, BITMASK_2BIT);
    } else {
        uint32_t tmp = READ_REG(RCC->BDCR);

        // Reset backup domain
        SET_BIT(RCC->BDCR, RCC_BDCR_BDRST);
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST);

        MODIFY_BITS(tmp, RCC_BDCR_RTCSEL_Pos, rtc_source, BITMASK_2BIT);

        WRITE_REG(RCC->BDCR, tmp);

        // Wait for LSE reactivation if it was enabled
        if (READ_BIT(RCC->BDCR, RCC_BDCR_LSEON)) {
            uint32_t tick_start = systick_get_tick();

            while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0) {
                if (systick_get_tick() - tick_start >=
                    PERIPHCLK_RTCSOURCELSE_TIMEOUT) {
                    return ERR_PERIPHCLK_RTCSEL_LSE_TIMEOUT;
                }
            }
        }
    }

    return OK;
}
