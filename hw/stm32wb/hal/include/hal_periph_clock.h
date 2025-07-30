#ifndef HAL_PERIPH_CLOCK_H
#define HAL_PERIPH_CLOCK_H

#include "hal_clock.h"
#include "hal_err.h"

typedef enum {
    PERIPHCLK_PLLSAI1_ADCCLK = RCC_PLLSAI1CFGR_PLLREN,
    PERIPHCLK_PLLSAI1_USBCLK = RCC_PLLSAI1CFGR_PLLQEN,
    PERIPHCLK_PLLSAI1_SAI1CLK = RCC_PLLSAI1CFGR_PLLPEN,
} periph_clock_pllsai1_output;

typedef struct {

    clock_plln plln;
    clock_pllp pllp;
    clock_pllq pllq;
    clock_pllr pllr;

    periph_clock_pllsai1_output output;

} periph_clock_pllsai1_t;

typedef enum {
    PERIPHCLK_UART_CLKSOURCE_PCLK = 0U,
    PERIPHCLK_UART_CLKSOURCE_SYSCLK = 1U,
    PERIPHCLK_UART_CLKSOURCE_HSI = 2U,
    PERIPHCLK_UART_CLKSOURCE_LSE = 3U,
} periph_clock_uart_source;

typedef enum {
    PERIPHCLK_I2C_PCLK = 0U,
    PERIPHCLK_I2C_SYSCLK = 1U,
    PERIPHCLK_I2C_HSI = 2U,
} periph_clock_i2c_source;

typedef enum {
    PERIPHCLK_LPTIM_PCLK = 0U,
    PERIPHCLK_LPTIM_LSI = 1U,
    PERIPHCLK_LPTIM_HSI = 2U,
    PERIPHCLK_LPTIM_LSE = 3U,
} periph_clock_lptim_source;

typedef enum {
    PERIPHCLK_SAI_PLLSAI = 0U,
    PERIPHCLK_SAI_PLL = 1U,
    PERIPHCLK_SAI_HSI = 2U,
    PERIPHCLK_SAI_EXTCLK = 3U,
} periph_clock_sai1_source;

typedef enum {
    PERIPHCLK_RNG_USB = 0U, // Use the same clk source as USB clock source in
                            // "periph_clock_usb_source"
    PERIPHCLK_RNG_LSI = 1U,
    PERIPHCLK_RNG_LSE = 2U,
} periph_clock_rng_source;

typedef enum {
    PERIPHCLK_USB_HSI48 = 0U,
    PERIPHCLK_USB_SAIPLLQ = 1U,
    PERIPHCLK_USB_PLLQ = 2U,
    PERIPHCLK_USB_MSI = 3U,
} periph_clock_usb_source;

typedef enum {
    PERIPHCLK_ADC_NONE = 0U,
#if defined(STM32WB55xx) || defined(STM32WB5Mxx) || defined(STM32WB35xx)
    PERIPHCLK_ADC_PLLSAI1 = 1U,
#elif defined(STM32WB15xx) || defined(STM32WB1Mxx)
    PERIPHCLK_ADC_HSI = 1U,
#endif // STM32WB55xx || STM32WB5Mxx || STM32WB35xx
    PERIPHCLK_ADC_PLL = 2U,
    PERIPHCLK_ADC_SYSCLK = 3U,
} periph_clock_adc_source;

typedef enum {
    PERIPHCLK_RTC_NONE = 0U,
    PERIPHCLK_RTC_LSE = 1U,
    PERIPHCLK_RTC_LSI = 2U,
    PERIPHCLK_RTC_HSEDIV32 = 3U,
} periph_clock_rtc_source;

typedef enum {
    PERIPHCLK_RFWAKEUP_NONE = 0U,
    PERIPHCLK_RFWAKEUP_LSE = 1U,
#if defined(STM32WB15xx) || defined(STM32WB10xx)
    PERIPHCLK_RFWAKEUP_LSI = 2U,
#endif // STM32WB15xx || STM32WB10xx
    PERIPHCLK_RFWAKEUP_HSEDIV1024 = 3U,
} periph_clock_rfwakeup_source;

typedef enum {
    PERIPHCLK_SMPS_HSI = 0U,
    PERIPHCLK_SMPS_MSI = 1U,
    PERIPHCLK_SMPS_HSE = 2U,
} periph_clock_smps_source;

typedef enum {
    PERIPHCLK_SMPS_DIV_0 = 0U,
    PERIPHCLK_SMPS_DIV_1 = 1U,
    PERIPHCLK_SMPS_DIV_2 = 2U,
    PERIPHCLK_SMPS_DIV_3 = 3U,
} periph_clock_smps_div;

#define PERIPHCLK_PLLSAI1_CONFIG_TIMEOUT 2U
#define PERIPHCLK_RTCSOURCELSE_TIMEOUT 5000U

hal_err periph_clock_config_pll_sai1(periph_clock_pllsai1_t pll_sai1);

static inline void
periph_clock_select_sai1_source(periph_clock_sai1_source sai1_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_SAI1SEL_Pos, sai1_source, BITMASK_2BIT);
}

static inline periph_clock_sai1_source periph_clock_get_sai1_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_SAI1SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_usart1_source(periph_clock_uart_source usart1_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_USART1SEL_Pos, usart1_source,
                BITMASK_2BIT);
}

static inline periph_clock_uart_source periph_clock_get_usart1_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_USART1SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_lpuart1_source(periph_clock_uart_source lpuart1_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_LPUART1SEL_Pos, lpuart1_source,
                BITMASK_2BIT);
}

static inline periph_clock_uart_source periph_clock_get_lpuart1_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_LPUART1SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_i2c1_source(periph_clock_i2c_source i2c1_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_I2C1SEL_Pos, i2c1_source, BITMASK_2BIT);
}

static inline periph_clock_i2c_source periph_clock_get_i2c1_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_I2C1SEL_Pos, BITMASK_2BIT);
}

static inline periph_clock_i2c_source periph_clock_get_i2c3_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_I2C3SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_lptim1_source(periph_clock_lptim_source lptim1_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_LPTIM1SEL_Pos, lptim1_source,
                BITMASK_2BIT);
}

static inline periph_clock_lptim_source periph_clock_get_lptim1_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_LPTIM1SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_lptim2_source(periph_clock_lptim_source lptim2_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_LPTIM2SEL_Pos, lptim2_source,
                BITMASK_2BIT);
}

static inline periph_clock_lptim_source periph_clock_get_lptim2_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_LPTIM2SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_usb_source(periph_clock_usb_source usb_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_CLK48SEL_Pos, usb_source, BITMASK_2BIT);
}

static inline periph_clock_usb_source periph_clock_get_usb_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_CLK48SEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_rng_source(periph_clock_rng_source rng_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_RNGSEL_Pos, rng_source, BITMASK_2BIT);
}

static inline periph_clock_rng_source periph_clock_get_rng_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_RNGSEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_adc_source(periph_clock_adc_source adc_source) {
    MODIFY_BITS(RCC->CCIPR, RCC_CCIPR_ADCSEL_Pos, adc_source, BITMASK_2BIT);
}

static inline periph_clock_adc_source periph_clock_get_adc_source() {
    return READ_BITS(RCC->CCIPR, RCC_CCIPR_ADCSEL_Pos, BITMASK_2BIT);
}

hal_err periph_clock_select_rtc_source(periph_clock_rtc_source rtc_source);

static inline periph_clock_rtc_source periph_clock_get_rtc_source() {
    return READ_BITS(RCC->BDCR, RCC_BDCR_RTCSEL_Pos, BITMASK_2BIT);
}

static inline void periph_clock_select_rfwakeup_source(
    periph_clock_rfwakeup_source rfwakeup_source) {
    MODIFY_BITS(RCC->CSR, RCC_CSR_RFWKPSEL_Pos, rfwakeup_source, BITMASK_2BIT);
}

static inline periph_clock_rfwakeup_source periph_clock_get_rfwakeup_source() {
    return READ_BITS(RCC->CSR, RCC_CSR_RFWKPSEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_smps_source(periph_clock_smps_source smps_source) {
    MODIFY_BITS(RCC->SMPSCR, RCC_SMPSCR_SMPSSEL_Pos, smps_source, BITMASK_2BIT);
}

static inline periph_clock_smps_source periph_clock_get_smps_source() {
    return READ_BITS(RCC->SMPSCR, RCC_SMPSCR_SMPSSEL_Pos, BITMASK_2BIT);
}

static inline void
periph_clock_select_smps_divider(periph_clock_smps_div smps_divider) {
    MODIFY_BITS(RCC->SMPSCR, RCC_SMPSCR_SMPSDIV_Pos, smps_divider,
                BITMASK_2BIT);
}

static inline periph_clock_smps_div periph_clock_get_smps_divider() {
    return READ_BITS(RCC->SMPSCR, RCC_SMPSCR_SMPSDIV_Pos, BITMASK_2BIT);
}

#endif // HAL_PERIPH_CLOCK_H
