#ifndef HAL_CLOCK_H
#define HAL_CLOCK_H

#include "hal_bits.h"
#include "stm32wbxx.h"

#include <stdint.h>

#ifndef HSI_VALUE
#define HSI_VALUE 16000000U
#endif // HSI_VALUE

#ifndef MSI_VALUE
#define MSI_VALUE 4000000U
#endif // MSI_VALUE

#ifndef HSE_VALUE
#define HSE_VALUE 32000000U
#endif // HSE_VALUE

#ifndef LSE_VALUE
#define LSE_VALUE 32768U
#endif // LSE_VALUE

typedef enum {
    CLOCK_SOURCE_MSI = 0U,
    CLOCK_SOURCE_HSI16 = 1U,
    CLOCK_SOURCE_HSE = 2U,
    CLOCK_SOURCE_PLL = 3U,
} clock_source;

typedef enum {
    CLOCK_PLL_SOURCE_NONE = 0U,
    CLOCK_PLL_SOURCE_MSI = 1U,
    CLOCK_PLL_SOURCE_HSI16 = 2U,
    CLOCK_PLL_SOURCE_HSE = 3U,
} clock_pll_source;

typedef enum {
    CLOCK_PLLM_1 = 0U,
    CLOCK_PLLM_2 = 1U,
    CLOCK_PLLM_3 = 2U,
    CLOCK_PLLM_4 = 3U,
    CLOCK_PLLM_5 = 4U,
    CLOCK_PLLM_6 = 5U,
    CLOCK_PLLM_7 = 6U,
    CLOCK_PLLM_8 = 7U,
} clock_pllm;

typedef enum {
    CLOCK_PLLN_6 = 6U,
    CLOCK_PLLN_7 = 7U,
    CLOCK_PLLN_8 = 8U,
    CLOCK_PLLN_9 = 9U,
    CLOCK_PLLN_10 = 10U,
    CLOCK_PLLN_11 = 11U,
    CLOCK_PLLN_12 = 12U,
    CLOCK_PLLN_13 = 13U,
    CLOCK_PLLN_14 = 14U,
    CLOCK_PLLN_15 = 15U,
    CLOCK_PLLN_16 = 16U,
    CLOCK_PLLN_17 = 17U,
    CLOCK_PLLN_18 = 18U,
    CLOCK_PLLN_19 = 19U,
    CLOCK_PLLN_20 = 20U,
    CLOCK_PLLN_21 = 21U,
    CLOCK_PLLN_22 = 22U,
    CLOCK_PLLN_23 = 23U,
    CLOCK_PLLN_24 = 24U,
    CLOCK_PLLN_25 = 25U,
    CLOCK_PLLN_26 = 26U,
    CLOCK_PLLN_27 = 27U,
    CLOCK_PLLN_28 = 28U,
    CLOCK_PLLN_29 = 29U,
    CLOCK_PLLN_30 = 30U,
    CLOCK_PLLN_31 = 31U,
    CLOCK_PLLN_32 = 32U,
    CLOCK_PLLN_33 = 33U,
    CLOCK_PLLN_34 = 34U,
    CLOCK_PLLN_35 = 35U,
    CLOCK_PLLN_36 = 36U,
    CLOCK_PLLN_37 = 37U,
    CLOCK_PLLN_38 = 38U,
    CLOCK_PLLN_39 = 39U,
    CLOCK_PLLN_40 = 40U,
    CLOCK_PLLN_41 = 41U,
    CLOCK_PLLN_42 = 42U,
    CLOCK_PLLN_43 = 43U,
    CLOCK_PLLN_44 = 44U,
    CLOCK_PLLN_45 = 45U,
    CLOCK_PLLN_46 = 46U,
    CLOCK_PLLN_47 = 47U,
    CLOCK_PLLN_48 = 48U,
    CLOCK_PLLN_49 = 49U,
    CLOCK_PLLN_50 = 50U,
    CLOCK_PLLN_51 = 51U,
    CLOCK_PLLN_52 = 52U,
    CLOCK_PLLN_53 = 53U,
    CLOCK_PLLN_54 = 54U,
    CLOCK_PLLN_55 = 55U,
    CLOCK_PLLN_56 = 56U,
    CLOCK_PLLN_57 = 57U,
    CLOCK_PLLN_58 = 58U,
    CLOCK_PLLN_59 = 59U,
    CLOCK_PLLN_60 = 60U,
    CLOCK_PLLN_61 = 61U,
    CLOCK_PLLN_62 = 62U,
    CLOCK_PLLN_63 = 63U,
    CLOCK_PLLN_64 = 64U,
    CLOCK_PLLN_65 = 65U,
    CLOCK_PLLN_66 = 66U,
    CLOCK_PLLN_67 = 67U,
    CLOCK_PLLN_68 = 68U,
    CLOCK_PLLN_69 = 69U,
    CLOCK_PLLN_70 = 70U,
    CLOCK_PLLN_71 = 71U,
    CLOCK_PLLN_72 = 72U,
    CLOCK_PLLN_73 = 73U,
    CLOCK_PLLN_74 = 74U,
    CLOCK_PLLN_75 = 75U,
    CLOCK_PLLN_76 = 76U,
    CLOCK_PLLN_77 = 77U,
    CLOCK_PLLN_78 = 78U,
    CLOCK_PLLN_79 = 79U,
    CLOCK_PLLN_80 = 80U,
    CLOCK_PLLN_81 = 81U,
    CLOCK_PLLN_82 = 82U,
    CLOCK_PLLN_83 = 83U,
    CLOCK_PLLN_84 = 84U,
    CLOCK_PLLN_85 = 85U,
    CLOCK_PLLN_86 = 86U,
    CLOCK_PLLN_87 = 87U,
    CLOCK_PLLN_88 = 88U,
    CLOCK_PLLN_89 = 89U,
    CLOCK_PLLN_90 = 90U,
    CLOCK_PLLN_91 = 91U,
    CLOCK_PLLN_92 = 92U,
    CLOCK_PLLN_93 = 93U,
    CLOCK_PLLN_94 = 94U,
    CLOCK_PLLN_95 = 95U,
    CLOCK_PLLN_96 = 96U,
    CLOCK_PLLN_97 = 97U,
    CLOCK_PLLN_98 = 98U,
    CLOCK_PLLN_99 = 99U,
    CLOCK_PLLN_100 = 100U,
    CLOCK_PLLN_101 = 101U,
    CLOCK_PLLN_102 = 102U,
    CLOCK_PLLN_103 = 103U,
    CLOCK_PLLN_104 = 104U,
    CLOCK_PLLN_105 = 105U,
    CLOCK_PLLN_106 = 106U,
    CLOCK_PLLN_107 = 107U,
    CLOCK_PLLN_108 = 108U,
    CLOCK_PLLN_109 = 109U,
    CLOCK_PLLN_110 = 110U,
    CLOCK_PLLN_111 = 111U,
    CLOCK_PLLN_112 = 112U,
    CLOCK_PLLN_113 = 113U,
    CLOCK_PLLN_114 = 114U,
    CLOCK_PLLN_115 = 115U,
    CLOCK_PLLN_116 = 116U,
    CLOCK_PLLN_117 = 117U,
    CLOCK_PLLN_118 = 118U,
    CLOCK_PLLN_119 = 119U,
    CLOCK_PLLN_120 = 120U,
    CLOCK_PLLN_121 = 121U,
    CLOCK_PLLN_122 = 122U,
    CLOCK_PLLN_123 = 123U,
    CLOCK_PLLN_124 = 124U,
    CLOCK_PLLN_125 = 125U,
    CLOCK_PLLN_126 = 126U,
    CLOCK_PLLN_127 = 127U,
} clock_plln;

typedef enum {
    CLOCK_PLLP_NONE = 0,
    CLOCK_PLLP_2 = 1,
    CLOCK_PLLP_3 = 2,
    CLOCK_PLLP_4 = 3,
    CLOCK_PLLP_5 = 4,
    CLOCK_PLLP_6 = 5,
    CLOCK_PLLP_7 = 6,
    CLOCK_PLLP_8 = 7,
    CLOCK_PLLP_9 = 8,
    CLOCK_PLLP_10 = 9,
    CLOCK_PLLP_11 = 10,
    CLOCK_PLLP_12 = 11,
    CLOCK_PLLP_13 = 12,
    CLOCK_PLLP_14 = 13,
    CLOCK_PLLP_15 = 14,
    CLOCK_PLLP_16 = 15,
    CLOCK_PLLP_17 = 16,
    CLOCK_PLLP_18 = 17,
    CLOCK_PLLP_19 = 18,
    CLOCK_PLLP_20 = 19,
    CLOCK_PLLP_21 = 20,
    CLOCK_PLLP_22 = 21,
    CLOCK_PLLP_23 = 22,
    CLOCK_PLLP_24 = 23,
    CLOCK_PLLP_25 = 24,
    CLOCK_PLLP_26 = 25,
    CLOCK_PLLP_27 = 26,
    CLOCK_PLLP_28 = 27,
    CLOCK_PLLP_29 = 28,
    CLOCK_PLLP_30 = 29,
    CLOCK_PLLP_31 = 30,
    CLOCK_PLLP_32 = 31,
} clock_pllp;

typedef enum {
    CLOCK_PLLQ_NONE = 0U,
    CLOCK_PLLQ_2 = 1U,
    CLOCK_PLLQ_3 = 2U,
    CLOCK_PLLQ_4 = 3U,
    CLOCK_PLLQ_5 = 4U,
    CLOCK_PLLQ_6 = 5U,
    CLOCK_PLLQ_7 = 6U,
    CLOCK_PLLQ_8 = 7U,
} clock_pllq;

typedef enum {
    CLOCK_PLLR_NONE = 0U,
    CLOCK_PLLR_2 = 1U,
    CLOCK_PLLR_3 = 2U,
    CLOCK_PLLR_4 = 3U,
    CLOCK_PLLR_5 = 4U,
    CLOCK_PLLR_6 = 5U,
    CLOCK_PLLR_7 = 6U,
    CLOCK_PLLR_8 = 7U,
} clock_pllr;

typedef struct {

    clock_pll_source source;

    clock_pllm pllm;
    clock_pllp pllp;
    clock_plln plln;
    clock_pllq pllq;
    clock_pllr pllr;

} clock_pll_config_t;

typedef enum {
    CLOCK_HCLK2_PRESC_DIV_3 = 1U,
    CLOCK_HCLK2_PRESC_DIV_5 = 2U,
    CLOCK_HCLK2_PRESC_DIV_6 = 5U,
    CLOCK_HCLK2_PRESC_DIV_10 = 6U,
    CLOCK_HCLK2_PRESC_DIV_32 = 7U,
    CLOCK_HCLK2_PRESC_DIV_2 = 8U,
    CLOCK_HCLK2_PRESC_DIV_4 = 9U,
    CLOCK_HCLK2_PRESC_DIV_8 = 10U,
    CLOCK_HCLK2_PRESC_DIV_16 = 11U,
    CLOCK_HCLK2_PRESC_DIV_64 = 12U,
    CLOCK_HCLK2_PRESC_DIV_128 = 13U,
    CLOCK_HCLK2_PRESC_DIV_256 = 14U,
    CLOCK_HCLK2_PRESC_DIV_512 = 15U,
} clock_hclk2_prescaler;

typedef enum {
    CLOCK_USB_RNG_SOURCE_HSI48 = 0U,
    CLOCK_USB_RNG_SOURCE_PLLSAI1Q = 1U,
    CLOCK_USB_RNG_SOURCE_PLLQ = 2U,
    CLOCK_USB_RNG_SOURCE_MSI = 3U,
} clock_usb_rng_source;

// Global
uint32_t clock_get_system_clock();

void clock_hclk2_set_prescaler(clock_hclk2_prescaler presc);
clock_hclk2_prescaler clock_hclk2_get_prescaler();

static inline void clock_select_source(clock_source source) {
    MODIFY_BITS(RCC->CFGR, RCC_CFGR_SW_Pos, source, BITMASK_2BIT);
    while (READ_BITS(RCC->CFGR, RCC_CFGR_SWS_Pos, BITMASK_2BIT) != source) {
    }
}

uint32_t clock_get_hclk_frequency();

uint32_t clock_get_pclk1_frequency();
uint32_t clock_get_pclk2_frequency();

// HSE
void clock_hse_enable();

// PLL
void clock_pll_config(const clock_pll_config_t *config);
void clock_pll_update_config(clock_pll_config_t *config);
void clock_pll_enable();

// USB
static inline void clock_usb_enable() {
    SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USBEN);
}

static inline void clock_usb_disable() {
    CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USBEN);
}

static inline bool clock_usb_enabled() {
    return READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USBEN);
}

// CRS
static inline void clock_crs_enable() {
    SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CRSEN);
}

static inline void clock_crs_disable() {
    CLEAR_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CRSEN);
}

static inline bool clock_crs_enabled() {
    return READ_BIT(RCC->APB1ENR1, RCC_APB1ENR1_CRSEN);
}

// ADC
static inline void clock_adc_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);
}

static inline void clock_adc_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);
}

static inline bool clock_adc_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);
}

// DMA1
static inline void clock_dma1_enable() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);
}

static inline void clock_dma1_disable() {
    CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);
}

static inline bool clock_dma1_enabled() {
    return READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);
}

// DMA2
static inline void clock_dma2_enable() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);
}

static inline void clock_dma2_disable() {
    CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);
}

static inline bool clock_dma2_enabled() {
    return READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);
}

// DMAMUX
static inline void clock_dmamux_enable() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMAMUX1EN);
}

static inline void clock_dmamux_disable() {
    CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMAMUX1EN);
}

static inline bool clock_dmamux_enabled() {
    return READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMAMUX1EN);
}

// GPIO

static inline void clock_gpioa_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
}

static inline void clock_gpioa_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
}

static inline bool clock_gpioa_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
}

static inline void clock_gpiob_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN);
}

static inline void clock_gpiob_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN);
}

static inline bool clock_gpiob_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN);
}

static inline void clock_gpioc_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
}

static inline void clock_gpioc_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
}

static inline bool clock_gpioc_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN);
}

static inline void clock_gpiod_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN);
}

static inline void clock_gpiod_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN);
}

static inline bool clock_gpiod_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN);
}

static inline void clock_gpioe_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN);
}

static inline void clock_gpioe_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN);
}

static inline bool clock_gpioe_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN);
}

static inline void clock_gpioh_enable() {
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN);
}

static inline void clock_gpioh_disable() {
    CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN);
}

static inline bool clock_gpioh_enabled() {
    return READ_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOHEN);
}

// HSEM
static inline void clock_hsem_enable() {
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_HSEMEN);
}

static inline void clock_hsem_disable() {
    CLEAR_BIT(RCC->AHB3ENR, RCC_AHB3ENR_HSEMEN);
}

static inline bool clock_hsem_enabled() {
    return READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_HSEMEN);
}

// IPCC
static inline void clock_ipcc_enable() {
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN);
    // delay after turning clocks on
    __IO uint32_t tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN);
    (void)tmp;
}

static inline void clock_ipcc_disable() {
    CLEAR_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN);
    // delay after turning clocks off
    __IO uint32_t tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN);
    (void)tmp;
}

static inline bool clock_ipcc_enabled() {
    return READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_IPCCEN);
}

// USART1

static inline void clock_usart1_enable() {
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
}

static inline void clock_usart1_disable() {
    CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
}

static inline bool clock_usart1_enabled() {
    return READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
}

// LPUART1

static inline void clock_lpuart1_enable() {
    SET_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN);
}

static inline void clock_lpuart1_disable() {
    CLEAR_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN);
}

static inline bool clock_lpuart1_enabled() {
    return READ_BIT(RCC->APB1ENR2, RCC_APB1ENR2_LPUART1EN);
}

#endif // HAL_CLOCK_H
